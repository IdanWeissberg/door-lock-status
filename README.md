# Door Lock Status
Starter structure for a door-lock status indicator using a **microswitch (SPDT lever)** and Arduino/ESP32.

## Why this skeleton?
- Clean folders: `hardware/`, `firmware/`, `docs/`, `test/`
- Ready `.gitignore`
- Simple CI (GitHub Actions)
- Keep code comments short and in English

## Folders
- `hardware/` – wiring, BOM, photos.
- `firmware/` – MCU code (Arduino/ESP32).
- `docs/` – notes and decisions.
- `test/` – quick checks.

## Roadmap 
- [ ] MVP: LED shows locked/unlocked
- [ ] Buzzer when door left open
- [ ] Optional: phone notification
- [ ] Enclosure tidy-up

## License
MIT 

Setup confirmed via GitHub Desktop.

## Architecture (Block Diagram)
![Block diagram](hardware/door-lock-block.png)

## Real wiring (MVP)
![Real wiring — ESP32 DevKitC, microswitch COM→GPIO21, NO→GND](hardware/microswitch-wiring.png)

**Caption:** ESP32 DevKitC with microswitch **COM→GPIO21**, **NO→GND** (GPIO21 as `INPUT_PULLUP`).  
LED on **GPIO2 → 220Ω → GND**.  
Logic: **LOCKED = HIGH**, **UNLOCKED = LOW**. Debounce: **40 ms**.  
*(If your latch presses the lever when locked, swap NO↔NC to keep LOCKED = HIGH.)*


**Setup (photo):**
- **Board:** ESP32 DevKitC
- **Sensor:** Microswitch (SPDT lever), wired so that **LOCKED reads HIGH** with `INPUT_PULLUP`.
  - Use **COM → GND** and connect **GPIO21** to the switch terminal **NO** that is **open** in the locked state (i.e., the pin is **not** pulled to GND when locked).
  - Practical hint:
    - If the lever is **not pressed** when the door is locked → use **NO → GPIO21**.
    - If the lever **is pressed** when the door is locked → use **NC → GPIO21**.
- **ESP32 pin config:** `SENSOR_PIN = GPIO21` configured as `INPUT_PULLUP`
- **LED:** `GPIO2` (onboard) — or external LED: `GPIO2 → 220Ω → LED anode → GND`
- **Power:** USB 5V
- **Logic (INPUT_PULLUP):** **LOCKED → HIGH**, **UNLOCKED → LOW**
- **Debounce:** 40 ms




