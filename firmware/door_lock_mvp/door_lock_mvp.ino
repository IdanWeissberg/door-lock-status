/*
  Door Lock Status (ESP32 + microswitch)
  PURPOSE: Read a microswitch (GPIO21), drive LED (GPIO2), and serve a phone UI.
  NOTES:
    - Serial prints CLOSED/OPEN (raw switch semantics).
    - HTTP /status returns Unlock/Locked (UI wording for the phone UI).
    - Uses mDNS → access via http://door.local (no need for fixed IP in router).
*/

#include <WiFi.h>
#include "secrets.h"     // Defines WIFI_SSID / WIFI_PASS (keep this file local; don't commit)
#include <WebServer.h>   // Lightweight HTTP server (ESP32 Arduino core)
#include <ESPmDNS.h>     // mDNS for 'door.local'
#include <ArduinoIoTCloud.h>          // [ADD] Cloud core
#include "arduino_secrets.h"          // [ADD] 
#include "thingProperties.h"          // [ADD] 



// --- Network (static IP attempt; DHCP fallback in code below) ---
IPAddress local_IP(192,168,1,70);
IPAddress gateway(192,168,1,1);
IPAddress subnet(255,255,255,0);
IPAddress dns(192,168,1,1);

// --- Minimal mobile-first UI (served at GET /) ---
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
ip.textContent=location.host; // show host (IP or door.local)
async function fetchStatus(){
  try{
    const r=await fetch('/status',{cache:'no-store'}); // avoid stale responses
    const t=(await r.text()).trim(); // "Locked" | "Unlock"
    box.textContent=t;
    document.body.className=(t==='Locked')?'locked':'unlocked'; // update color
    upd.textContent='Last update: '+new Date().toLocaleTimeString();
  }catch(e){
    box.textContent='—'; upd.textContent='Last update: error';
  }
}
btn.addEventListener('click',fetchStatus);
fetchStatus(); setInterval(fetchStatus,500); // auto-refresh every 0.5s
</script>
</body></html>
)rawliteral";

WebServer server(80); // HTTP listens on port 80

// --- Hardware pins ---
const int SENSOR  = 21; // Microswitch → GPIO21 to GND (INPUT_PULLUP active-low)
const int LED_PIN = 2;  // LED (through ~220Ω) to GND; HIGH = LED ON

// --- Debounce state ---
const unsigned long DEBOUNCE_MS = 40; // Non-blocking debounce window
int lastReading;       // Last raw sample (can bounce)
int stableState;       // Debounced (committed) state
unsigned long lastChangeMs; // Timestamp when raw change first seen

// Serve HTML page at "/"
void handleRoot(){
  // Use String() to avoid overload issues across core versions
  server.send(200, "text/html", String(INDEX_HTML));
}

// Plain-text state for the phone UI at "/status"
// IMPORTANT mapping: LOW => "Unlock", HIGH => "Locked" (as per your UI wording)
void handleStatus() {
  server.sendHeader("Cache-Control", "no-store");
  server.send(200, "text/plain", (stableState == LOW) ? "Unlock" : "Locked");
}

// Wi-Fi connect: try static IP first; if association/timeout → DHCP fallback
bool connectWiFi() {
  WiFi.persistent(false);          // avoid writing creds to flash
  WiFi.disconnect(true, true);     // clear previous state
  WiFi.mode(WIFI_OFF);             // hard reset radio (helps stuck states)
  delay(200);
  WiFi.mode(WIFI_STA);             // station mode (join existing Wi-Fi)
  WiFi.setSleep(false);            // keep radio awake for responsive HTTP
  WiFi.setAutoReconnect(true);     // auto-reconnect on drops

  // --- Attempt static IP (your preference) ---
  WiFi.setHostname("door");        // hostname (helps router & mDNS)
  bool ok = WiFi.config(local_IP, gateway, subnet, dns);
  Serial.print("WiFi.config: "); Serial.println(ok ? "OK" : "FAIL");

  WiFi.begin(WIFI_SSID, WIFI_PASS);
  Serial.print("WiFi (static): connecting");
  unsigned long t0 = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - t0 < 15000) {
    Serial.print(".");
    delay(250);
  }
  Serial.println();

  if (WiFi.status() == WL_CONNECTED) {
    Serial.print("WiFi: connected (static), IP = ");
    Serial.println(WiFi.localIP()); // expected 192.168.1.70 if router allows
    return true;
  }

  // --- DHCP fallback: keeps UI available even if static is refused ---
  Serial.println("WiFi: static failed → DHCP fallback");
  WiFi.disconnect(true, true);
  delay(200);
  WiFi.mode(WIFI_STA);
  WiFi.setHostname("door");        // set again (safe)
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  Serial.print("WiFi (DHCP): connecting");
  t0 = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - t0 < 15000) {
    Serial.print(".");
    delay(250);
  }
  Serial.println();

  if (WiFi.status() == WL_CONNECTED) {
    Serial.print("WiFi: connected (DHCP), IP = ");
    Serial.println(WiFi.localIP());
    return true;
  }

  Serial.println("WiFi: FAILED (timeout)"); // check SSID/pass, 2.4GHz, isolation, etc.
  return false;
}

void setup() {
  Serial.begin(115200); // diagnostics baud rate

  // --- Wi-Fi bring-up ---
  connectWiFi();
  initProperties();                                        // [ADD]
  ArduinoCloud.begin(ArduinoIoTPreferredConnection);       // [ADD]
  setDebugMessageLevel(2); ArduinoCloud.printDebugInfo();  // [ADD] 


  // --- HTTP routes ---
  server.on("/", handleRoot);
  server.on("/status", handleStatus);
  server.onNotFound([](){ server.send(404, "text/plain", "Not found"); });
  server.begin(); // start HTTP server

  // --- mDNS: 'http://door.local' name ---
  if (MDNS.begin("door")) {
    MDNS.addService("http", "tcp", 80);
    Serial.println("mDNS: http://door.local");
  } else {
    Serial.println("mDNS: FAILED");
  }
  Serial.print("MAC: "); Serial.println(WiFi.macAddress()); // for a future DHCP reservation if you want

  // --- GPIO init ---
  pinMode(SENSOR, INPUT_PULLUP); // active-low input; idle HIGH when open
  pinMode(LED_PIN, OUTPUT);      // LED output

  // Initial read → set LED + one-time Serial line (CLOSED/OPEN)
  if (digitalRead(SENSOR) == LOW) {
    digitalWrite(LED_PIN, HIGH);  // LED ON
    Serial.println("CLOSED");
  } else {
    digitalWrite(LED_PIN, LOW);   // LED OFF
    Serial.println("OPEN");
  }

  // Debounce baseline
  lastReading  = digitalRead(SENSOR);
  stableState  = lastReading;
  lastChangeMs = millis();
  lock = (stableState == HIGH);  
}

void loop() {
  ArduinoCloud.update();                                   

  // --- Non-blocking debounce ---
  int reading = digitalRead(SENSOR); // raw sample each loop

  // if raw changed → restart debounce window
  if (reading != lastReading) {
    lastReading  = reading;
    lastChangeMs = millis();
  }

  // commit when stable long enough and differs from current stableState
  if ((millis() - lastChangeMs) >= DEBOUNCE_MS &&
      reading == lastReading &&
      lastReading != stableState) {

    stableState = lastReading; // accept debounced state

    // Apply LED + Serial (CLOSED/OPEN wording)
    if (stableState == LOW) {
      digitalWrite(LED_PIN, HIGH); // LED ON
      Serial.println("CLOSED");
    } else {
      digitalWrite(LED_PIN, LOW);  // LED OFF
      Serial.println("OPEN");
    }
    lock = (stableState == HIGH);
  }

  // --- Serve any pending HTTP requests ---
  server.handleClient();

  // no delay(); keep loop responsive
} 