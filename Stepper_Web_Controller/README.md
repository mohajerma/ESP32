# Stepper Web Controller

A small Arduino app for controlling a NEMA 17 bipolar stepper through an **A4988** or **DRV8825** driver. It supports both ESP8266 and ESP32 boards and exposes a responsive WiFi control page for jogging, speed tests, enable/disable, stop, and repeatable moves.

## Features

- ESP8266 and ESP32 Arduino core compatibility
- No external Arduino libraries
- Access point mode by default, so it works without a router
- Optional station mode for an existing WiFi network
- Non-blocking STEP pulse generation so the web server remains responsive
- Live position, motion, speed, and driver status
- Optional end-stop inputs with compile-time enable
- Browser controls for jog, arbitrary distance, one-revolution cycle, direction, and pulse tests

## Hardware

- ESP8266 NodeMCU or ESP32 DevKit board
- NEMA 17 bipolar stepper motor
- A4988 or DRV8825 carrier
- Separate motor supply appropriate for the motor and driver
- 100 uF or larger electrolytic capacitor across VMOT and GND at the driver
- Optional normally-closed end-stop switches

Do **not** power the motor from the ESP board's 3.3 V pin or USB supply. Tie the ESP ground, driver ground, and motor-supply ground together.

## Upload

Open `Stepper_Web_Controller.ino` in Arduino IDE or compile from the workspace's Arduino CLI:

```bat
C:\code\ESP32\Tools\arduino-cli.exe compile --fqbn esp32:esp32:esp32 C:\code\ESP32\Stepper_Web_Controller
C:\code\ESP32\Tools\arduino-cli.exe compile --fqbn esp8266:esp8266:generic C:\code\ESP32\Stepper_Web_Controller
```

Upload with the correct port, for example:

```bat
C:\code\ESP32\Tools\arduino-cli.exe upload -p COM3 --fqbn esp32:esp32:esp32 C:\code\ESP32\Stepper_Web_Controller
```

Change the FQBN for an ESP8266 upload. Open Serial Monitor at **115200 baud**. The default access point is:

- SSID: `StepperBench`
- Password: `stepper123`
- URL: `http://192.168.4.1`

## Configuration

Edit the constants near the top of the sketch:

- `STEP_PIN`, `DIR_PIN`, and `ENABLE_PIN` for your board wiring
- `USE_ACCESS_POINT` to `0` for existing WiFi
- `STA_SSID` and `STA_PASSWORD` for station mode
- `USE_LIMIT_SWITCHES` to `1` only after wiring the switches
- `MAX_SPEED` to keep testing within the motor/driver limits

The cycle test assumes 200 full steps per revolution. With 1/16 microstepping, one revolution is 3200 pulses, so use the move-distance field for microstepped tests.

## Safety

Start with a low current limit and low speed. Secure the motor and load before testing. Stop immediately if the driver, motor, or wiring becomes hot. Never connect or disconnect the motor while the driver is powered. The web stop button stops commanded pulses; it is not an emergency-stop circuit. Add a physical power cutoff for hazardous machinery.

See [WIRING_GUIDE.md](WIRING_GUIDE.md) for pinouts and driver wiring.
