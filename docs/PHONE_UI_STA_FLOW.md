# Phone UI (Local Web over STA) — Flow

## Overview
Goal: View the door state on a phone over the home Wi-Fi (no internet).  
State strings are exactly: **Locked** / **Unlock**.

## Network Mode
- **STA**: ESP32 joins the home Wi-Fi; phone is on the same LAN.
- IP is assigned by the router (DHCP).

## Endpoints (HTTP)
- `GET /` → minimal page that auto-refreshes the state.
- `GET /status` → `200 OK`, `Content-Type: text/plain`,
  `Cache-Control: no-store`, body: `Locked` or `Unlock`.

## Logic Mapping
- SENSOR = GPIO21, configured as **INPUT_PULLUP**.
- **LOW → Locked (LED ON)**, **HIGH → Unlock (LED OFF)**.
- Debounce ≈ **40 ms** on state transitions.

## Timing
- Page polling interval: ~**500 ms**.
- Acceptance: UI reflects state change within **≤ 1 s** (debounce + polling).

## Flow (high-level)
Phone (browser) ──(same Wi-Fi/LAN)──> ESP32 (HTTP server)

1) ESP32 boots → joins home Wi-Fi (gets IP via DHCP).
2) User opens `http://<ESP32-IP>/` on the phone.
3) `GET /` returns a minimal page.
4) Page periodically sends `GET /status` (no cache).
5) ESP32 reads debounced sensor state → responds:
   - `Locked` if stable state is LOW
   - `Unlock` if stable state is HIGH
6) Page updates the on-screen label accordingly.

## Manual Test Checklist
- [ ] Phone + ESP32 on the same Wi-Fi.
- [ ] Open `http://<ESP32-IP>/` → page loads.
- [ ] Toggle door/switch → label updates to **Locked/Unlock** ≤ 1 s.
- [ ] Open `http://<ESP32-IP>/status` directly → exact `Locked` or `Unlock`.
- [ ] Unknown path (e.g., `/xyz`) → `404 Not Found`.
- [ ] Works without internet (LAN only).

## Troubleshooting
- IP changed? Check Serial output, router UI, or use a LAN scanner.
- Phone prefers mobile data → toggle airplane mode ON → Wi-Fi ON.
- Use **http** (not https) for local access on LAN.

## Security (MVP)
- LAN-only access. No exposure to the internet.
- No login on the page at this stage (can add later if needed).
