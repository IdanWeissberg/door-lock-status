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

