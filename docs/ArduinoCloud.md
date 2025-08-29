# Arduino Cloud — Door Lock Dashboard

## Goal
Expose door state to Arduino Cloud so the dashboard shows:
- **Value widget** (text) bound to **`lockText`** (String): `"Locked"` / `"Unlock"`

## Cloud Variables (Thing)
- `isLocked`: **Bool**, READWRITE, **On change**  
  *Note:* Remote writes are ignored in firmware via `onIsLockedChange()`.
- `lockText`: **String**, READ, **On change**

## Dashboard (Widgets)
- **Value**  → bind to `lockText` (white text)

## Firmware touchpoints (reference)
- Includes: `ArduinoIoTCloud.h`, `arduino_secrets.h`, `thingProperties.h`
- In `setup()`:
  ```cpp
  initProperties();
  ArduinoCloud.begin(ArduinoIoTPreferredConnection);
