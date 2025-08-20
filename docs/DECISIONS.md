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

