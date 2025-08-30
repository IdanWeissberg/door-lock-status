# Test Log — Firmware MVP (ESP32)

Date: 2025-08-22
Board: ESP32 Dev Module
Baud: 115200

Results:
- Boot-closed: OK
- Open-toggle: OK
- 60s-idle: OK

# Test Log — Local HTTP UI (ESP32)

Date: 2025-08-30  
Board: ESP32 Dev Module  
Baud: 115200  
IP: 192.168.1.70 (static, example)

Results:
- Wi-Fi connect (<5 s): **OK** — STA joined, RSSI stable
- Static IP acquired: **OK** — 192.168.1.70
- GET `/` HTML (200): **OK** — page loads on desktop & phone
- GET `/status` (text/plain): **OK** — returns `Locked` / `Unlock`
- Cache headers: **OK** — `Cache-Control: no-store`
- LED mapping sanity: **OK** — SENSOR LOW → LED ON
- Debounce behavior: **OK** — <20 ms ignored, ≥60 ms registered
- State latency (UI/status): **OK** — ≤100 ms after stable change
- Phone access (same Wi-Fi): **OK** — iPhone Safari works
- Concurrent clients (PC+iPhone): **OK** — no stalls/timeouts
- Refresh hammer (5 req/s × 30 s): **OK** — no resets, 200s only
- Idle 5 min then fetch: **OK** — server responsive
- Power-cycle recovery: **OK** — server up ≤3 s post-boot
- Reset button (no physical change): **OK** — no false flips
- Security (LAN only): **OK** — STA mode, no WAN exposure/forwarding

Notes:
- Status semantics locked: **SENSOR LOW → `Unlock`/`Locked` strings per project decision** (kept consistent with firmware docs).

# Test Log — Arduino IoT Cloud (ESP32)

Date: 2025-08-30  
Board: ESP32 Dev Module  
Baud: 115200  
Thing: DoorLock (Arduino IoT Cloud)  
Properties: `isLocked` (bool, read-only), `lockText` (string, read-only)

Results:
- Cloud connect (<10 s): **OK** — `ArduinoCloud.begin()` online
- Initial publish after boot: **OK** — state visible ≤2 s after connect
- Hardware → Cloud sync: **OK** — toggling lever updates `isLocked`
- `lockText` mapping: **OK** — “Locked” / “Unlock” correct
- Updates on state change only: **OK** — no spam while idle
- Rapid toggle ×10 (~10 s): **OK** — all stable changes reflected
- Cloud write attempts ignored: **OK** — `onIsLockedChange()` no-op
- Temporary Wi-Fi loss (30 s): **OK** — auto-reconnect & resync
- HTTP UI & Cloud consistency: **OK** — values match across both
- Power-cycle with door CLOSED: **OK** — dashboard shows “Locked”
- Power-cycle with door OPEN: **OK** — dashboard shows “Unlock”
- Long idle (10 min): **OK** — connection healthy, no ghost flips

Notes:
- Debounce (40 ms) remains authoritative source for state; Cloud reflects debounced state only.


