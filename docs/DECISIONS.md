# Decisions Log
- Init project skeleton.
- Reed type: NO; magnet side/orientation: <fill after test>
- Pins: SENSOR = D2, LED = LED_BUILTIN (or D13 + 220Ω → GND)
- Input mode: INPUT_PULLUP; wiring: Reed D2 ↔ GND
- Logic: magnet close → SENSOR LOW; magnet away → SENSOR HIGH
- LED mapping: CLOSED=<ON/OFF>, OPEN=<ON/OFF>  (choose and fill)
