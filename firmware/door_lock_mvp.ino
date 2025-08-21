/*
  Door Lock Status (ESP32 + microswitch)
  - Purpose: Debounced read of a microswitch on GPIO21 and LED indication on GPIO2
  - Wiring: SENSOR → GPIO21 to GND (uses INPUT_PULLUP), LED → GPIO2 through 220Ω to GND
  - Logic: ACTIVE-LOW input -> LOW = CLOSED (pressed), HIGH = OPEN (released)
  - Serial: 115200 baud, prints state only on debounced changes
  - Debounce: timing-based, non-blocking (~40 ms)
*/

// Pins
const int SENSOR  = 21;   // Microswitch to GND (active-low with INPUT_PULLUP)
const int LED_PIN = 2;    // LED (220Ω to GND), HIGH = ON, LOW = OFF

// Timing
const unsigned long DEBOUNCE_MS = 40; // Debounce window in milliseconds

// Debounce state
int lastReading;              // Last raw reading (may bounce)
int stableState;              // Debounced/committed state (what LED/Serial reflect)
unsigned long lastChangeMs;   // Timestamp when a raw change was first seen

void setup() {
  Serial.begin(115200);            // Open USB serial at 115200 baud
  pinMode(SENSOR, INPUT_PULLUP);   // Enable internal pull-up; input idles HIGH (open)
  pinMode(LED_PIN, OUTPUT);        // LED pin as push-pull output

  // Initial mapping: read once and set LED + initial print
  if (digitalRead(SENSOR) == LOW) {    // LOW => CLOSED
    digitalWrite(LED_PIN, HIGH);       // LED ON for CLOSED
    Serial.println("CLOSED");          // Initial state print
  } else {                             
    digitalWrite(LED_PIN, LOW);        // LED OFF for OPEN
    Serial.println("OPEN");            // Initial state print
  }

  // Initialize debounce variables to current input/time
  lastReading  = digitalRead(SENSOR);  // Raw baseline
  stableState  = lastReading;          // Debounced baseline
  lastChangeMs = millis();             // Start timing reference
}

void loop() {
  int reading = digitalRead(SENSOR);               // Single raw read per loop

  // Raw edge detect: if raw value changed, restart debounce timer
  if (reading != lastReading) {
    lastReading  = reading;                        // Track the new raw level
    lastChangeMs = millis();                       // Reset debounce stopwatch
  }

  // Accept change only if:
  //  (1) raw stayed the same long enough, (2) still equal to lastReading, (3) it's a new state
  if ((millis() - lastChangeMs) >= DEBOUNCE_MS &&
      reading == lastReading &&
      lastReading != stableState) {

    stableState = lastReading;                     // Commit debounced state

    if (stableState == LOW) {                      // CLOSED (active-low)
      digitalWrite(LED_PIN, HIGH);                 // LED ON
      Serial.println("CLOSED");                    // Print on change only
    } else {                                       // OPEN
      digitalWrite(LED_PIN, LOW);                  // LED OFF
      Serial.println("OPEN");                      // Print on change only
    }
  }

  // Note: no delay(); loop remains non-blocking
}
