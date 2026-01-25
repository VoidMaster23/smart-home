# 🧠 The C++ Smart Home Controller

Yo. This is a bare-metal, smart home system written in **C++20**.
I could have gone for home assistant. BUT I chose to rather build this thing brick by brick

This project connects Zigbee lights (IKEA, Hue, etc.) to a custom-built C++ control loop via MQTT. It's Dockerized, so you don't have to deal with dependency hell.

## 🏗 The Architecture
We running a "Holy Trinity" setup:
1.  **The Ears (Zigbee2MQTT):** Translates radio waves from zigbee enabled devices.
2.  **The Messenger (Mosquitto):** The broker that passes notes between connected devices.
3.  **The Brain (C++):** Custom binary that makes the decisions.

## 🎒 Prerequisites
You barely need anything.
* A computer (I use a rasperry pi since i dont wanna have my PC on all the time )
* A Zigbee USB Stick (use only if you need to use zigbee, otherwise tyou shoudl be able to get away with plain and simple MQTT instead)
* Docker & Docker Compose (`curl -fsSL https://get.docker.com -o get-docker.sh && sudo sh get-docker.sh`)

## 📂 The Structure
We use a "CppDevBase" pattern. You just drop files, and the system finds them.

```text
smart-home/
├── Makefile             # The command center (build, logs, shell)
├── generate_sources.py  # Python script that auto-updates CMake lists
├── docker-compose.yaml  # Infrastructure definition
└── src/
    ├── CMakeLists.txt   # The build recipe
    ├── main.cpp         # The entry point
    ├── include/         # Drop your .hpp headers here
    └── [Your Code]      # Drop .cpp files anywhere here## 

## 🚀 How to Run This Mf
We built this to be clone-and-go.

### 1. Clone the Repo
```bash
git clone 
cd smart-home
````

### 2. Plug in the hardware
Plug in your zigbee stick to your computer. I used a USB extension cable because i was getting connection issues. This is optional though. I just think I had gaaaaang interference.


### 3. Identify the dongle
```bash
ls -l /dev/serial/by-id/
```
This will output something like: 
```bash
 usb-Silicon_Labs_CP2102_USB_to_UART_Bridge_Controller_0001-if00-port0 ⇒ ../../ttyUSB0
 ```

 Update the `devices` section in the `docker-compose.yaml` 
to have a be something like:
 ```yml
  devices:
      - /dev/serial/by-id/<left-side-of-output>:/dev/<the identified port, mine is on ttyUSB0>
 ```



### 4. COnfigure Zigbee
Ngl i was too lazy to clean up my configuration file

So, create the file at `infrastructure/zigbee2mqtt/data/configuration.yaml`

and paste this mf in:
```yaml
# Home Assistant integration (MQTT discovery)
homeassistant: false

# Allow new devices to join
permit_join: true

# MQTT Broker Settings
mqtt:
  base_topic: zigbee2mqtt
  server: 'mqtt://mosquitto:1883'

# Serial Adapter Settings
serial:
  port: /dev/<your port>
  adapter: zstack

# Frontend (The Dashboard)
frontend:
  port: 8080

```



 ### 5. Let it riiiiiiop
run the following mf:
 ```bash
make build # this handles compilation and starts the containers
 ```

 if you want to see logs for you can just run
 ```bash
 docker compose logs -f <service>
 ```

 At this point you need to go to http://<localhost or hostname of your pi if using one>:8080
 1. click "permit join"
 2. put your zigbee devices in pairing mode (to be fair, I only tested this on lights but i mean surely this should work for other stuff)
 3. code to your hearts content. Now tyou can just run a `docker compose up -d --build` to make sure everything is setup correctly
 **Note**:  Edit files in src/. The CMake system automatically finds new .cpp files, so just create them and run docker compose up -d --build to recompile.



## 🛠 Technical Debt & Future Refactors
This project is currently in the prototype phase. The following items are identified for future architectural hardening and modernization to align with C++20 and professional Qt standards:

1. Memory & Ownership
 - [ ] Transition to Stack Allocation: Replace heap-allocated UI components (new QLabel) with member variables to satisfy clang-tidy (gsl::owner) and improve cache locality.

- [ ] Smart Pointer Migration: Replace remaining raw pointers for non-Qt objects with `std::unique_ptr` or `std::shared_ptr`.

- [ ] Virtual Destructors: Audit base classes (e.g., Device, Sensor) to ensure destructors are virtual, preventing partial destruction during polymorphic deletes.

2. Type Safety & Performance
- [ ] Const Correctness: Audit all "getters" and observer methods to mark them as const and [[nodiscard]].

- [ ] String Optimization: Replace `std::string` or QString pass-by-value with `std::string_view` (C++17/20) for read-only device names and identifiers.

- [ ] Explicit Casting: Replace `dynamic_cast` with `qobject_cast` for QObject-derived types to bypass RTTI overhead and improve safety across library boundaries.

3. Modern C++20 Patterns
- [ ] Concepts & Constraints: Implement C++20 Concepts for template-based device managers to provide better compile-time errors.

- [ ] Designated Initializers: Refactor device configuration structs to use C++20 designated initialization (.name = "Kitchen") for better readability.

- [ ] Signal/Slot Modernization: Ensure all connect() calls use the pointer-to-member-function syntax for compile-time signature verification.

4. UI/UX Refinements
- [ ] Text Elision: Implement a custom ElidedLabel using `QFontMetrics::elidedText` to handle long device names gracefully in constrained layouts.

- [ ] High-DPI Scaling: Audit layouts to ensure they utilize QLayout scaling rather than fixed pixel dimensions.