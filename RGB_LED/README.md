# ESP32 RGB LED Control Project

This Arduino project demonstrates RGB LED control using an ESP32 Dev Module with PWM (Pulse Width Modulation).

## Hardware Requirements

- **ESP32 Dev Module** (any variant with USB programming capability)
- **RGB LED** (common cathode) or 3 separate LEDs (Red, Green, Blue)
- **3x Resistors** (220Ω - 330Ω depending on your LEDs)
- **Breadboard and jumper wires**

## Circuit Connections

Connect the RGB LED to your ESP32 as follows:

| LED Pin | ESP32 GPIO | Resistor |
|---------|------------|----------|
| Red     | GPIO 25    | 220-330Ω |
| Green   | GPIO 26    | 220-330Ω |
| Blue    | GPIO 27    | 220-330Ω |
| Ground  | GND        | -        |

**Note:** If using a common cathode RGB LED, connect the common pin to GND. For common anode, you'll need to invert the logic in the code.

## Software Setup

### 1. Install Arduino IDE

Download and install the Arduino IDE from [arduino.cc](https://www.arduino.cc/en/software)

### 2. Install ESP32 Board Support

1. Open Arduino IDE
2. Go to **File → Preferences**
3. Add the following URL to "Additional Board Manager URLs":
   ```
   https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
   ```
4. Go to **Tools → Board → Boards Manager**
5. Search for "ESP32" and install **"esp32 by Espressif Systems"**

### 3. Configure Board Settings

1. Select **Tools → Board → ESP32 Arduino → ESP32 Dev Module**
2. Configure the following settings:
   - **Upload Speed:** 921600
   - **CPU Frequency:** 240MHz (WiFi/BT)
   - **Flash Frequency:** 80MHz
   - **Flash Mode:** QIO
   - **Flash Size:** 4MB (32Mb)
   - **Partition Scheme:** Default 4MB with spiffs
   - **Core Debug Level:** None
   - **PSRAM:** Disabled
3. Select your **COM Port** under **Tools → Port**

### 4. Upload the Sketch

1. Open `RGB_LED.ino` in Arduino IDE
2. Click the **Upload** button (→) or press `Ctrl+U`
3. Wait for compilation and upload to complete

## Project Structure

```
RGB LED/
├── RGB_LED.ino        # Main Arduino sketch
├── config.h           # Pin definitions and configuration
└── README.md          # This file
```

## Features

- PWM-based RGB LED control (8-bit resolution, 256 levels per color)
- Color cycling through primary and secondary colors
- Rainbow fade effect
- Serial monitor output for debugging
- Easy pin configuration through config.h

## Usage

### Serial Monitor

Open the Serial Monitor (**Tools → Serial Monitor**) at **115200 baud** to see status messages and current color information.

### Customization

Edit `config.h` to:
- Change GPIO pins for RGB connections
- Adjust serial baud rate
- Add WiFi credentials for network projects
- Configure other constants

Edit `RGB_LED.ino` to:
- Modify color sequences
- Adjust timing delays
- Add new LED effects
- Implement additional functionality

## Troubleshooting

### Upload Issues

- **COM Port not found:** Install CP2102 or CH340 USB driver (depends on your ESP32 board)
- **Upload fails:** Hold the BOOT button on ESP32 while uploading
- **Permission denied:** Close Serial Monitor before uploading

### LED Issues

- **LED doesn't light up:** Check wiring and resistor values
- **Wrong colors:** Verify RGB pin connections match config.h
- **Too dim/bright:** Adjust resistor values or LED_BRIGHTNESS in config.h

### ESP32 Driver Installation

- **Windows:** [CP210x Driver](https://www.silabs.com/developers/usb-to-uart-bridge-vcp-drivers) or [CH340 Driver](http://www.wch-ic.com/downloads/CH341SER_ZIP.html)
- **macOS:** Usually works without additional drivers
- **Linux:** Add user to dialout group: `sudo usermod -a -G dialout $USER`

## Common ESP32 GPIO Notes

### Safe Pins for General Use

GPIO that are safe to use: 1, 3, 4, 16, 17, 18, 19, 21, 22, 23, 25, 26, 27, 32, 33

### Pins to Avoid

- **GPIO 0:** Used for boot mode selection
- **GPIO 2:** Connected to onboard LED (usually)
- **GPIO 5:** Used for SPI
- **GPIO 6-11:** Connected to flash memory (do not use)
- **GPIO 12:** Boot fails if pulled high
- **GPIO 15:** Outputs PWM at boot

### Input Only Pins

GPIO 34, 35, 36, 39 are input only (no internal pull-up/pull-down)

## Expanding the Project

Ideas for enhancement:
- Add web server for WiFi control
- Implement Bluetooth control via smartphone
- Add potentiometers for manual color adjustment
- Create custom lighting patterns
- Add button controls for mode selection
- Integrate with home automation systems

## License

This project is open source and available for educational and personal use.

## Resources

- [ESP32 Official Documentation](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/)
- [Arduino ESP32 Core](https://github.com/espressif/arduino-esp32)
- [ESP32 Pinout Reference](https://randomnerdtutorials.com/esp32-pinout-reference-gpios/)
