# Decisions Log
- Init project skeleton.
- Board: Arduino Uno (also OK with ESP32-DevKitC; use LED_BUILTIN).
- Reed switch: type = NO (normally-open).
  Magnet orientation: label the magnet face that closes the switch at ~5–10 mm; mount so **door CLOSED → switch closed**.
- Pins: SENSOR = D2, LED = LED_BUILTIN (fallback: D13 + 220Ω → GND).
- Input mode: INPUT_PULLUP; wiring: REED between D2 and GND.
- Logic (with INPUT_PULLUP):
  - Magnet close (door CLOSED/locked)  → SENSOR = LOW
  - Magnet away  (door OPEN/unlocked) → SENSOR = HIGH
- LED mapping: CLOSED = ON, OPEN = OFF. (Invert later if you prefer the opposite.)
- Debounce: 40 ms (minimum time between accepted state changes).
- Serial: 115200 baud; print "OPEN"/"CLOSED" only on changes.
- Power (dev): 5V via USB from PC/phone charger (no external PSU needed).
- Artifacts to add: `hardware/wiring.png` (diagram), `hardware/wiring-photo.jpg` (real wiring).
