# Door Lock Firmware — MVP (ESP32)

## Overview (Why)
Minimal, reliable door-status indication using a reed/micro switch.
Stable input with `INPUT_PULLUP`, simple 40 ms debounce, and clear Serial logs.

## Pinout (What)
- SENSOR: GPIO21 (INPUT_PULLUP)
- LED: GPIO2
- Logic: CLOSED=LOW → LED ON, OPEN=HIGH → LED OFF

## Build & Flash 
1. Board: ESP32 Dev Module
2. Tools → Port: your ESP32 port
3. Serial Monitor: 115200 baud
4. Upload and observe `OPEN`/`CLOSED` printed only on state changes.
5. Serial Monitor baud: 115200


## Debounce
Timing-based (40 ms). Increase to 60–80 ms if you see chatter.

## Troubleshooting
- Always HIGH? Check wiring to GND and `INPUT_PULLUP`.
- Flapping logs? Lift debounce to 60–80 ms and verify mechanical stability.
- LED inverted? Confirm logic mapping above.

## License
MIT

