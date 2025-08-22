# Wi-Fi STA Setup (ESP32 joins the home network)

## Goal
Run the phone UI over the home Wi-Fi (STA mode). The phone and ESP32 are on the same LAN; the UI shows **Locked / Unlock**.

## Requirements
- Home Wi-Fi credentials (SSID + password).
- 2.4 GHz network available (ESP32 works best on 2.4 GHz).
- Phone connected to the same LAN as the ESP32.

## Network Checklist
- Prefer **WPA2-PSK** security (avoid open networks/captive portals).
- Make sure the SSID is **visible** (hidden SSIDs complicate first connect).
- If your router has band steering (2.4 GHz + 5 GHz same name), it’s fine, but ensure 2.4 GHz is enabled.
- DHCP enabled on the router (default).

## Optional (recommended): DHCP Reservation
To avoid the ESP32 IP changing after reboots:
1. Find the ESP32 **MAC address** (printed on boot in Serial, or in router’s client list).
2. In the router UI, add a **DHCP reservation** so the ESP32 always gets the same local IP (e.g., `192.168.1.50`).
3. Save/apply and reboot the ESP32 if needed.

## How you will access the UI
- Browser on the phone → `http://<ESP32-IP>/`
- Direct status endpoint → `http://<ESP32-IP>/status` returns **Locked** or **Unlock** (`text/plain`, not cached).

## Local Secrets Policy
- Store Wi-Fi credentials **locally** (not in the repo).
- The repository `.gitignore` excludes your local secrets file to prevent accidental commits.

## Troubleshooting
- **Can’t load the page:** Confirm phone and ESP32 are on the same LAN; recheck the IP via router/Serial.
- **No IP assigned:** Ensure DHCP is on; check Wi-Fi password; try 2.4 GHz only.
- **Intermittent access:** Consider a DHCP reservation; reduce Wi-Fi channel crowding in the router.
- **Phone prefers mobile data:** Toggle airplane mode ON, then Wi-Fi ON.
- **HTTPS warning:** Use **http** for local LAN access; HTTPS is out of scope for the MVP.

## Acceptance Reminder
- UI reflects **Locked / Unlock** within ≤ 1 s (includes debounce + polling).
