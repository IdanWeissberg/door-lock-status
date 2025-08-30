# Door Lock Status
Starter structure for a door-lock status indicator using a **microswitch (SPDT lever)** on an **ESP32**. Clean, minimal, and documented.

---

## Why → What → How (at a glance)
- **Why:** Know if the door is **Locked/Unlock** at home and (optionally) in Arduino Cloud.
- **What:** Read a microswitch on **GPIO21**, debounce, map to **Locked/Unlock**, mirror on LED (**GPIO2**), serve a tiny phone UI.
- **How:** Wire switch + LED → flash the firmware → browse `http://door.local/` → (optional) bind Arduino Cloud dashboard widgets.

---

## Folders
- `hardware/` – wiring, BOM, photos.
- `firmware/` – MCU code (Arduino/ESP32).
- `docs/` – notes and decisions.
- `test/` – quick checks.

## Roadmap
- [x] MVP: LED + phone UI (`/`, `/status`)
- [x] Optional: Arduino Cloud dashboard (`isLocked`, `lockText`)
- [ ] Enclosure tidy-up / notification

## License
MIT

---

## Architecture (Block Diagram)
![Block diagram](hardware/door-lock-block.png)

## Real wiring (MVP)
![Real wiring — ESP32 DevKitC, microswitch COM→GPIO21, NO/NC→GND](hardware/microswitch-wiring.png)

**Wiring summary**
- **Sensor (microswitch):** `SENSOR_PIN = GPIO21` as `INPUT_PULLUP`
  - Typical: **COM → GPIO21**, **NO → GND** (lever **not** pressed when locked → pin is OPEN → **HIGH**)
  - If your latch presses the lever when locked, use **NC → GND** (to keep **LOCKED = HIGH**).
- **LED:** **GPIO2 → 220Ω → LED → GND**
- **Power:** USB 5V
- **Debounce:** 40 ms

**State mapping (project decision)**

| Physical (microswitch) | `digitalRead` | Logical | LED (GPIO2) | Serial (raw) | `/status` text |
|---|---:|---|---|---|---|
| Lever released (door locked) | `HIGH` | **Locked** | OFF | `OPEN` | `Locked` |
| Lever pressed (door unlock)  | `LOW`  | **Unlock** | ON  | `CLOSED` | `Unlock` |

> Notes: INPUT_PULLUP means idle/open = HIGH. We keep **LOCKED = HIGH**.

---

## Firmware (ESP32)
- Sketch path: `firmware/door_lock_mvp/door_lock_mvp.ino`
- Toolchain: Arduino IDE (ESP32 core), or Arduino CLI

**Secrets (keep local; don’t commit)**
- `secrets.h` → `#define WIFI_SSID "..."` / `#define WIFI_PASS "..."` (local Wi-Fi)
- `arduino_secrets.h` → Arduino Cloud credentials (if using Cloud)

**Key includes you will see in the sketch**
```cpp
#include <WiFi.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <ArduinoIoTCloud.h>
#include "secrets.h"           // local Wi-Fi (do not commit)
#include "arduino_secrets.h"   // Arduino Cloud (do not commit)
#include "thingProperties.h"   // Cloud variables & bindings
