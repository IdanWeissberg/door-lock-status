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
  - **COM → GPIO21**
  - **NO  → GND**  (lever NOT pressed when locked → pin is OPEN → HIGH)
  - If your latch **presses** the lever when locked, use **NC → GND** instead (to keep LOCKED = HIGH).
- **ESP32 pin config:** `SENSOR_PIN = GPIO21` configured as `INPUT_PULLUP`
- **LED:** `GPIO2` (onboard) — or external: `GPIO2 → 220Ω → LED anode → GND`
- **Power:** USB 5V
- **Logic (INPUT_PULLUP):** **LOCKED → HIGH**, **UNLOCKED → LOW**
- **Debounce:** 40 ms

## Phone UI (Local Web over STA) — Quickstart

**Goal:** View the door state from your phone over the home Wi-Fi.  
**Static IP (example):** 192.168.1.70

### Requirements
- ESP32 on 2.4 GHz Wi-Fi (WPA2-PSK).
- `secrets.h` locally with correct SSID/PASS (not committed).
- Firmware flashed with static IP set (e.g., 192.168.1.70).

### How to access
- From a phone on the same Wi-Fi: open `http://192.168.1.70/` (UI page — TBD)  
- Direct status endpoint: `http://192.168.1.70/status` → returns **Unlock** or **Locked** (`text/plain`, `Cache-Control: no-store`)

### Status semantics (final for MVP)
- **/status**
  - `Unlock` → sensor **LOW** (microswitch closed)  
  - `Locked` → sensor **HIGH** (microswitch open)
- **Serial logs** print `CLOSED` / `OPEN` (raw switch concept).  
  *Intentional mismatch for clearer UI wording.*

### Test plan (manual)
1. Power ESP32; confirm Serial shows `WiFi: connected, IP = 192.168.1.70`.
2. From phone: open `http://192.168.1.70/status` → see `Unlock` or `Locked`.
3. Toggle the microswitch → status changes within ≤ 1 s.
4. (When UI page `/` is added) open `http://192.168.1.70/` → label/color updates automatically.

### Troubleshooting
- Same LAN: phone and ESP32 must be on the same Wi-Fi/LAN.
- If page doesn’t load: recheck the IP; try airplane-mode ON → Wi-Fi ON (prevents cellular hijack).
- Use **http** (not https) on LAN.
- If IP conflicts: pick another static IP in the same subnet (e.g., 192.168.1.150), flash again.

### Security (MVP)
- LAN-only; no internet exposure.
- No login on the page at this stage.
