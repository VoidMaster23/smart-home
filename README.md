# 🧠 Smart Home

Yo. This is a home-built smart home system running on a Raspberry Pi 5.
I could have gone for Home Assistant. BUT I chose to build this thing brick by brick.

Zigbee lights (IKEA Kajplats, Hue, …) talk to Zigbee2MQTT, everything meets on an MQTT broker, a React dashboard controls it all from any browser, and a modern C++ service will handle rooms and automations.

> 🚧 **Mid-rebuild.** The old Qt UI is being replaced. The React dashboard is in progress, and the C++ controller is still the old Qt code until it gets gutted (see [Roadmap](#-roadmap)).

## 🏗 Architecture

```text
 Zigbee devices ──radio──► Zigbee2MQTT ──┐
                                         ▼
                              Mosquitto (MQTT broker)
                              :1883 TCP   :9001 WebSockets
                                ▲               ▲
              C++ controller ───┘               └─── React dashboard
         (rooms + automations, WIP)          (any browser: kiosk, phone, laptop)
```

1. **Zigbee2MQTT** translates Zigbee radio into MQTT. It is the source of truth for device state.
2. **Mosquitto** passes messages between everything.
3. **React dashboard** talks MQTT directly over WebSockets. No custom backend API.
4. **C++ controller** (coming back headless): rooms and automations on the MQTT bus.

Key ideas:

- **Devices announce themselves** with Home Assistant's MQTT discovery format (`homeassistant/…/config`). Nothing is hardcoded per device.
- **Capability-driven UI.** A light gets a brightness slider, color-temperature presets or a color picker only if it says it supports them. A new bulb model needs zero code.
- **Rooms are ours**, not Zigbee2MQTT groups (group state is unreliable).
- **No optimistic UI.** Buttons send commands; the UI only changes when the device reports its new state.

## 📂 Structure

```text
smart-home/
├── controller/          # C++ service (CMake, Dockerfile, clang-format/tidy)
├── frontend/            # React dashboard (Vite + TypeScript)
│   └── src/store/       # Zustand store, types, discovery parsing (+ tests)
├── infrastructure/
│   ├── mosquitto/       # Broker config (data/log are gitignored)
│   └── zigbee2mqtt/     # Z2M data + configuration.yaml (gitignored)
├── docker-compose.yaml
├── Makefile
└── smart-home.code-workspace   # open this in VS Code
```

## 🎒 Prerequisites

- A Raspberry Pi (or any always-on Linux box)
- A Zigbee USB dongle
- Docker + Docker Compose
- Node 24 for the frontend
- [Tailscale](https://tailscale.com/) if you want the dashboard outside the house

## 🚀 Setup

### 1. Clone

```bash
git clone https://github.com/VoidMaster23/smart-home.git
cd smart-home
```

### 2. Find the Zigbee dongle

Plug in the stick (a USB extension cable helps against interference), then:

```bash
ls -l /dev/serial/by-id/
```

Put the result in the `devices` section of `zigbee2mqtt` in `docker-compose.yaml`:

```yaml
devices:
  - /dev/serial/by-id/<your-dongle>:/dev/ttyUSB0
```

### 3. Configure Zigbee2MQTT

`infrastructure/zigbee2mqtt/data/configuration.yaml` is gitignored (it holds network keys), so create it yourself. The parts this project relies on:

```yaml
mqtt:
  base_topic: zigbee2mqtt
  server: mqtt://mosquitto:1883

serial:
  port: /dev/ttyUSB0
  adapter: zstack

frontend:
  enabled: true
  port: 8080

# The dashboard discovers devices through HA MQTT discovery.
homeassistant:
  enabled: true

# Per-device online/offline (otherwise only the bridge reports availability).
availability:
  enabled: true
```

Check the [Z2M configuration docs](https://www.zigbee2mqtt.io/guide/configuration/) for the rest (network keys, channel). Don't create Z2M groups: rooms are handled by this project.

### 4. Start the broker and Zigbee2MQTT

```bash
docker compose up -d mqtt zigbee2mqtt
```

> ⚠️ Don't run `make build` / plain `docker compose up` for now: that also starts the old Qt controller.

Logs: `docker compose logs -f zigbee2mqtt`. After changing `configuration.yaml`: `docker compose restart zigbee2mqtt`.

### 5. Pair devices

Open `http://<pi-hostname>:8080` (the Z2M frontend), click **Permit join**, and put your devices in pairing mode.

### 6. Run the dashboard

```bash
cd frontend
npm install
npm run dev
```

The dashboard connects to the broker at `ws://<same host as the page>:9001`, so the same build works on the kiosk, on the LAN and over Tailscale.

## 🛠 Development

Open `smart-home.code-workspace` in VS Code (works over Remote-SSH on the Pi). Install the [Oxc extension](https://marketplace.visualstudio.com/items?itemName=oxc.oxc-vscode) for lint + format on save.

Frontend (`frontend/`):

| Command             | What                         |
| ------------------- | ---------------------------- |
| `npm run dev`       | Dev server                   |
| `npm test`          | Vitest                       |
| `npm run lint`      | oxlint                       |
| `npm run fix`       | oxlint --fix + oxfmt         |
| `npm run fmt:check` | Check formatting (CI style)  |
| `npm run build`     | Typecheck + production build |

Style: frontend uses oxlint + oxfmt; C++ follows the [Google C++ Style Guide](https://google.github.io/styleguide/cppguide.html) completely (`.clang-format`, clang-tidy, cpplint). `make lint` runs everything, but currently fails on the old Qt code.

## 🗺 Roadmap

- [x] **Phase 0:** MQTT over WebSockets, HA discovery, Tailscale, monorepo layout
- [ ] **Phase 1:** React dashboard: device store ✅, MQTT client, light controls, Home screen (clock, weather, news), nginx container for the kiosk
- [ ] **Phase 2:** Gut Qt. Headless C++23 controller (Paho MQTT, `std::expected`, `std::jthread`)
- [ ] **Phase 3:** Rooms (`rooms.json`, aggregated room state, room commands)
- [ ] **Phase 4:** Automations (schedules, sensor rules)

The old Qt version lives on the `archive/qt-ui` tag.
