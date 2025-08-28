/*
  Door Lock Status (ESP32 + microswitch + Arduino IoT Cloud)
  - Reads microswitch on GPIO21 (INPUT_PULLUP), drives LED on GPIO2.
  - Local phone UI: http://door.local (mDNS) | /status returns "Unlock"/"Locked".
  - Cloud: publishes boolean 'locked' (true = Locked) on debounced changes.
*/

#include <WiFi.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <ArduinoIoTCloud.h>     // Cloud core
#include "thingProperties.h"     // <-- from Arduino IoT Cloud (must exist in this sketch)

// ---------------- HTML (served at "/") ----------------
static const char INDEX_HTML[] PROGMEM = R"rawliteral(
<!doctype html><html lang="en"><head>
<meta charset="utf-8"><meta name="viewport" content="width=device-width,initial-scale=1">
<title>Door</title>
<style>
  :root{--bg:#0e1116;--card:#151922;--ok:#18a558;--bad:#d64545;--txt:#e6e8eb;--muted:#9aa4ad}
  *{box-sizing:border-box} html,body{height:100%} body{margin:0;background:var(--bg);color:var(--txt);font-family:system-ui,Segoe UI,Roboto,Arial}
  .wrap{max-width:560px;margin:0 auto;padding:24px;display:flex;min-height:100%;align-items:center}
  .card{width:100%;background:var(--card);border-radius:24px;padding:28px;box-shadow:0 10px 36px rgba(0,0,0,.28)}
  h1{margin:0 0 6px;font-size:28px;font-weight:800;letter-spacing:.3px}
  p.m{margin:0 0 18px;color:var(--muted);font-size:14px}
  .status{display:flex;align-items:center;justify-content:center;height:150px;border-radius:18px;font-size:34px;font-weight:900;letter-spacing:.5px}
  .locked .status{background:var(--bad);color:#fff}
  .unlocked .status{background:var(--ok);color:#fff}
  .meta{margin-top:16px;display:flex;gap:8px;flex-wrap:wrap;color:var(--muted);font-size:12px}
  .chip{padding:6px 10px;border-radius:999px;background:#0d1016;border:1px solid #232a36}
  .row{display:flex;gap:10px;align-items:center;justify-content:space-between;margin-top:12px}
  button{appearance:none;border:0;border-radius:12px;padding:10px 14px;font-weight:700;cursor:pointer}
  .ghost{background:#0d1016;border:1px solid #232a36;color:#fff}
</style>
</head><body>
  <div class="wrap"><div class="card">
    <h1>Door</h1><p class="m">Live status over your home Wi-Fi</p>
    <div id="statusBox" class="status">—</div>
    <div class="row">
      <div class="meta">
        <span class="chip" id="ip">—</span>
        <span class="chip" id="updated">Last update: —</span>
        <span class="chip">Auto-refresh: 0.5s</span>
      </div>
      <button class="ghost" id="refresh">Refresh</button>
    </div>
  </div></div>
<script>
const box=document.getElementById('statusBox');
const ip=document.getElementById('ip');
const upd=document.getElementById('updated');
const btn=document.getElementById('refresh');
ip.textContent=location.host;
async function fetchStatus(){
  try{
    const r=await fetch('/status',{cache:'no-store'});
    const t=(await r.text()).trim(); // "Locked" | "Unlock"
    box.textContent=t;
    document.body.className=(t==='Locked')?'locked':'unlocked';
    upd.textContent='Last update: '+new Date().toLocaleTimeString();
  }catch(e){ box.textContent='—'; upd.textContent='Last update: error'; }
}
btn.addEventListener('click',fetchStatus);
fetchStatus(); setInterval(fetchStatus,500);
</script>
</body></html>
)rawliteral";

// ---------------- HTTP server ----------------
WebServer server(80);

// ---------------- Pins & debounce ----------------
const int SENSOR  = 21;   // microswitch → GPIO21 to GND (active-low)
const int LED_PIN = 2;    // LED (through ~220Ω) to GND; HIGH = ON
const unsigned long DEBOUNCE_MS = 40;

int lastReading;              // latest raw read
int stableState;              // debounced state (LOW/HIGH)
unsigned long lastChangeMs;   // when a raw change first seen

// ---------------- Handlers ----------------
void handleRoot() {
  server.send(200, "text/html", String(INDEX_HTML));
}

// UI wording: LOW => "Unlock", HIGH => "Locked"
void handleStatus() {
  server.sendHeader("Cache-Control", "no-store");
  server.send(200, "text/plain", (stableState == LOW) ? "Unlock" : "Locked");
}

void setup() {
  Serial.begin(115200);

  // --- Cloud bring-up (manages Wi-Fi) ---
  initProperties();                                      // binds Cloud vars (e.g., 'locked')
  ArduinoCloud.begin(ArduinoIoTPreferredConnection);     // connect to Arduino IoT Cloud
  setDebugMessageLevel(2); ArduinoCloud.printDebugInfo();

  // Optionally wait a bit for Wi-Fi before starting services
  unsigned long t0 = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - t0 < 15000) delay(100);

  // --- HTTP endpoints ---
  server.on("/", handleRoot);
  server.on("/status", handleStatus);
  server.onNotFound([](){ server.send(404, "text/plain", "Not found"); });
  server.begin();

  // --- mDNS (door.local) ---
  if (MDNS.begin("door")) {
    MDNS.addService("http", "tcp", 80);
    Serial.println("mDNS: http://door.local");
  } else {
    Serial.println("mDNS: FAILED");
  }
  Serial.print("MAC: "); Serial.println(WiFi.macAddress());

  // --- GPIOs ---
  pinMode(SENSOR, INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);

  // Initial state → LED + Serial + Cloud
  int first = digitalRead(SENSOR);
  stableState  = first;
  lastReading  = first;
  lastChangeMs = millis();

  if (first == LOW) { digitalWrite(LED_PIN, HIGH); Serial.println("CLOSED"); }
  else              { digitalWrite(LED_PIN, LOW ); Serial.println("OPEN");   }

  // Cloud semantics: true = Locked (sensor HIGH)
  locked = (first == HIGH);
}

void loop() {
  ArduinoCloud.update();  // keep Cloud connection alive & sync variables

  // ---- Non-blocking debounce ----
  int reading = digitalRead(SENSOR);
  if (reading != lastReading) {        // raw edge detected → restart window
    lastReading  = reading;
    lastChangeMs = millis();
  }

  if ((millis() - lastChangeMs) >= DEBOUNCE_MS &&
      reading == lastReading &&
      lastReading != stableState) {

    stableState = lastReading;         // commit debounced state

    // LED + Serial (raw semantics)
    if (stableState == LOW) { digitalWrite(LED_PIN, HIGH); Serial.println("CLOSED"); }
    else                    { digitalWrite(LED_PIN, LOW ); Serial.println("OPEN");   }

    // Publish to Cloud once per stable change (true = Locked)
    locked = (stableState == HIGH);
  }

  server.handleClient();  // serve HTTP
}
