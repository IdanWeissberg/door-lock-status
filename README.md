# Door Lock Status (חיווי נעילת דלת בריח)

Starter structure for a door-lock status indicator using a reed switch + magnet and Arduino/ESP32.

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

## Roadmap (example)
- [ ] MVP: LED shows locked/unlocked
- [ ] Buzzer when door left open
- [ ] Optional: phone notification
- [ ] Enclosure tidy-up

## License
MIT (already in your repo).

Setup confirmed via GitHub Desktop.

## Architecture (Block Diagram)
![Block diagram]([hardware/block-diagram.png](https://github.com/IdanWeissberg/door-lock-status-/blob/ee64db2f3af5de420b7cbdb71d391a4176ac5055/hardware/door-lock-wiring.png))

