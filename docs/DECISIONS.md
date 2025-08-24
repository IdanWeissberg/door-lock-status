# Decisions Log
- Init project skeleton.
- Board: ESP32 DevKitC.
- Sensor: Microswitch (SPDT lever).
- Input mode: INPUT_PULLUP.
- Wiring: COM → GPIO21, NO → GND (swap to NC if the lever is pressed when locked).
- Logic (with INPUT_PULLUP): LOCKED = HIGH (lever NOT pressed), UNLOCKED = LOW (lever pressed).
- Pins: SENSOR = GPIO21, LED = GPIO2 (or onboard LED if present).
- LED mapping: LOCKED = LED ON, UNLOCKED = LED OFF. (Invert in firmware if preferred.)
- Debounce: 40 ms (minimum time between accepted state changes).
- Serial: 115200 baud; print "LOCKED"/"UNLOCKED" only on changes.
- Power (dev): 5 V via USB (no external PSU required).
- Optional noise suppression: 100 nF from GPIO21 to GND for long/noisy runs.
- Artifacts to add: `hardware/door-lock-block.png`, `hardware/microswitch-wiring.png`, `hardware/microswitch-mount.jpg`.
## 2025-08-22 — Sensor baseline for MVP
- Input device: generic switch on GPIO21 with INPUT_PULLUP (compatible with reed or micro-switch).
- Logic mapping: CLOSED=LOW → LED ON (GPIO2).
- Debounce: 40 ms (tunable if mechanical chatter observed).
- Rationale: keep firmware agnostic to the mechanical sensor; final choice deferred to mechanical stage.
## Mobile UI — Local Web (STA)
- Network mode: Station (ESP32 joins home Wi-Fi; phone on same LAN).
- Status strings (exact): "Locked" / "Unlock".
- Logic mapping: SENSOR=GPIO21, INPUT_PULLUP; LOW=Locked (LED ON), HIGH=Unlock (LED OFF); debounce=40ms.
- HTTP endpoints:
  - GET / → minimal page that polls `/status` every 500 ms.
  - GET /status → 200 text/plain; Cache-Control: no-store; body: Locked | Unlock.
## Phone UI — Status Semantics (FINAL for MVP)
- Network: STA (ESP32 joins home Wi-Fi). Static IP used (e.g., 192.168.1.70).
- `/status` returns exact strings: `Unlock` when SENSOR=LOW, `Locked` when SENSOR=HIGH.
- Serial labels remain `CLOSED/OPEN` per raw switch state (active-low). This mismatch is intentional for UI wording.

