# 🎨 LED Matrix Display Controller

An advanced ESP32 project for controlling addressable RGB LED strips with moving text and graphics via WiFi and Bluetooth.

## ✨ Features

- 🌈 **Multiple Visual Effects**: Rainbow, fire, confetti, theater chase, and more
- 📝 **Scrolling Text**: Display custom messages with smooth scrolling
- 📱 **Dual Control**: WiFi web interface and Bluetooth Low Energy (BLE)
- 🎨 **Customizable**: Adjustable brightness, speed, and colors
- 🔧 **Easy Configuration**: Single config file for all settings
- 🚀 **FastLED Library**: High-performance LED control

## 📋 Hardware Requirements

### Required Components
- **ESP32 Development Board** (ESP32-WROOM-32, ESP32-DevKitC, or similar)
- **WS2812B LED Strip** (or compatible: WS2811, SK6812, APA102)
- **5V Power Supply** (sufficient amperage for your LED count)
  - Rule of thumb: 60mA per LED at full brightness
  - Example: 60 LEDs = 3.6A minimum
  - ⚠️ USB chargers may need triggering - see [Wiring Guide](WIRING_GUIDE.md#usb-charger-triggering)
- **1000µF Capacitor** (recommended for power stability)
- **Jumper Wires**

**Note on Data Line Resistor:**
- A 470Ω resistor is traditionally recommended but may cause signal issues
- For short wire runs (< 1m), resistor can be omitted
- See [Wiring Guide](WIRING_GUIDE.md) for details

### Optional Components
- 470Ω Resistor (for data line, may not be needed)
- Logic level shifter (3.3V to 5V) for better signal integrity
- Heat sink for ESP32 (if running high-power applications)

## 🔌 Wiring Diagram

```
ESP32                  LED Strip
-----                  ---------
GPIO 5 (configurable) ----[470Ω]----> DI (Data In)
GND -----------------------------> GND
5V Power Supply --------------> 5V/VCC
       |
       +--[1000µF Cap]--+
       |                |
      (+)              (-)
```

### Important Notes
- **Never power LED strips directly from ESP32** - Use an external 5V power supply
- Connect all grounds together (ESP32, LEDs, and power supply)
- Add a 1000µF capacitor across the power supply terminals
- Data line resistor is optional - see [Wiring Guide](WIRING_GUIDE.md#step-4-data-line-connection)
- USB chargers may require triggering - see [Wiring Guide](WIRING_GUIDE.md#usb-charger-triggering)

**📖 Complete wiring instructions: [WIRING_GUIDE.md](WIRING_GUIDE.md)**

## 🛠️ Software Setup

### Step 1: Install Arduino IDE

1. Download and install [Arduino IDE](https://www.arduino.cc/en/software) (version 2.0 or later)

### Step 2: Install ESP32 Board Support

1. Open Arduino IDE
2. Go to **File → Preferences**
3. Add this URL to "Additional Board Manager URLs":
   ```
   https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
   ```
4. Go to **Tools → Board → Boards Manager**
5. Search for "ESP32" and install **"ESP32 by Espressif Systems"**

### Step 3: Install Required Libraries

Install these libraries via **Tools → Manage Libraries** (Ctrl+Shift+I):

1. **FastLED** by Daniel Garcia
   - Search: "FastLED"
   - Install the latest version

The following libraries should be pre-installed with ESP32 board support:
- WiFi
- WebServer
- BLEDevice

### Step 4: Configure the Project

1. Open `config.h` in Arduino IDE
2. Update these essential settings:

```cpp
// LED Configuration
#define NUM_LEDS 60              // Change to your LED count
#define LED_PIN 5                // GPIO pin connected to LEDs

// WiFi Credentials
#define WIFI_SSID "YourWiFiName"
#define WIFI_PASSWORD "YourWiFiPassword"

// Enable/Disable Features
#define ENABLE_WIFI true
#define ENABLE_BLUETOOTH true
```

### Step 5: Upload to ESP32

1. Connect your ESP32 via USB
2. Select your board: **Tools → Board → ESP32 Arduino → ESP32 Dev Module**
3. Select the correct port: **Tools → Port → COM# (your ESP32 port)**
4. Click **Upload** (→ button) or press Ctrl+U

## 📱 Usage

### WiFi Web Interface

1. After uploading, open Serial Monitor (115200 baud)
2. Note the IP address di (rainbow, fire, confetti, etc.)
   - Enter scrolling text
   - Adjust brightness (0-255)
   - Change animation speed
   - **🆕 Configure LED count and matrix dimensions** (requires restart)
   - Enter scrolling text
   - Adjust brightness (0-255)
   - Change animation speed

### Bluetooth Control

1. Download a BLE Serial Terminal app on your phone:
   - Android: **"Serial Bluetooth Terminal"** or **"BLE Terminal"**
   - iOS: **"LightBlue"** or **"BLE Terminal HM-10"**

2. Connect to device: **"LED_Matrix_Display"**

3. Send commands in this format: `CMD:VALUE`

#### Available BLE Commands

```
MODE:rainbow          - Rainbow cycling effect
MODE:scroll_text      - Scrolling text display
MODE:color_wipe       - Color wipe animation
MODE:theater_chase    - Theater chase effect
MODE:fire            - Fire simulation
MODE:confetti        - Random confetti sparkles
MODE:solid           - Solid white light
MODE:off             - Turn off all LEDs

TEXT:Hello World     - Set scrolling text
BRIGHTNESS:200       - Set brightness (0-255)
SPEED:100           - Set animation speed (10-500ms)
COLOR:255,0,0       - Set RGB color (R,G,B values)
```

#### Example BLE Session
```
MODE:rainbow
TEXT:ESP32 Rocks!
BRIGHTNESS:150
SPEED:75
MODE:scroll_text
```

## 🎨 Available Effects

| Effect | Description |
|--------|-------------|
| **Rainbow** | Smooth rainbow color cycling |
| **Scroll Text** | Custom scrolling messages |
| **Color Wipe** | Color fills the strip sequentially |
| **Theater Chase** | Marquee-style chase effect |
| **Fire** | Realistic fire simulation |
| **Confetti** | Random colored sparkles |
| **Solid** | Solid white illumination |
| **Off** | All LEDs turned off |

## ⚙️ Advanced Configuration
LED Configuration via Web Interface 🆕

You can now configure LED matrix settings through the web interface:

1. **Matrix Dimensions**: Set width and height for matrix layouts
2. **Total LEDs**: Automatically calculated as width × height
3. Changes are saved and applied after ESP32 restart

**Note**: These settings override values in `config.h` after first configuration.

### Matrix Display Setup

If you're using LEDs arranged in a 2D matrix (grid):

```cpp
// In config.h
#define MATRIX_WIDTH 10
#define MATRIX_HEIGHT 6
#define MATRIX_TYPE VERTICAL_ZIGZAG
```

**Understanding Matrix Coordinates:**
- See [MATRIX_GUIDE.md](MATRIX_GUIDE.md) for complete explanation
- Formula: `ledIndex = row * MATRIX_WIDTH + column`
- Supports zigzag and straight wiring patternsfine MATRIX_TYPE VERTICAL_ZIGZAG
```

### Power Optimization

For battery operation, reduce brightness:

```cpp
#define MAX_BRIGHTNESS 128    // Reduce from 255
#define DEFAULT_BRIGHTNESS 64  // Lower default
```

### Access Point Mode

To create a standalone WiFi network:
   - ⚠️ USB chargers may need triggering (see [Wiring Guide](WIRING_GUIDE.md))

2. **Verify wiring**
   - Data line to correct GPIO pin (default: GPIO 14)
   - Check LED strip polarity (5V, GND, DI)
   - **Try removing data line resistor if present**

3. **Check config.h**
   - Correct `NUM_LEDS` value
   - Correct `LED_PIN` (default GPIO 14isit: `http://192.168.4.1`

## 🐛 Troubleshooting

### LEDs Not Lighting Up

1. **Check power supply**
   - Ensure 5V supply has sufficient current
   - Verify all ground connections

2. **Verify wiring**
   - Data line to correct GPIO pin
   - Check LED strip polarity (5V, GND, DI)

3. **Check config.h**
   - Correct `NUM_LEDS` value
   - Correct `LED_PIN` (default GPIO 5)
   - Correct `LED_TYPE` (WS2812B, WS2811, etc.)

### WiFi Connection Failed

1. Check Serial Monitor (115200 baud) for error messages
2. Verify WiFi credentials in `config.h`
3. Ensure 2.4GHz WiFi (ESP32 doesn't support 5GHz)
4. Move ESP32 closer to router

### Bluetooth Not Connecting

1. Ensure Bluetooth is enabled in `config.h`
2. Try restarting the ESP32
3. Clear Bluetooth cache on your phone
4. Check Serial Monitor for BLE initialization messages

### Compilation Errors

1. **"FastLED.h not found"**
   - Install FastLED library via Library Manager

2. **"WiFi.h not found"**
   - Verify ESP32 board support is installed

3. **Out of memory errors**
   - Reduce `NUM_LEDS` if memory is limited
   - Disable unused features (WiFi or Bluetooth)

### Performance Issues

1. **Flickering LEDs**
   - Add/check 1000µF capacitor across power supply
   - Use shorter wires
   - Add 470Ω resistor on data line

2. **Slow animations**
   - Increase FPS in `config.h`
   - Reduce `NUM_LEDS` if too many

## 📊 Power Consumption

| Configuration | Estimated Current |
|---------------|------------------|
| 60 LEDs @ 50% brightness | ~1.8A |
| 60 LEDs @ full white | ~3.6A |
| ESP32 only | ~200mA |

**Formula**: Current (A) = (NUM_LEDS × 0.06 × brightness_factor) + 0.2

## 🚀 Future Enhancements

- [ ] MQTT support for home automation
- [ ] Audio reactive modes (with microphone)
- [ ] Pattern creator web interface
- [ ] Smartphone app (iOS/Android)
- [ ] Preset saving/loading
- [ ] Schedule-based automation
- [ ] Multiple strip support
- [ ] Weather display integration

## 📖 API Reference

### Control Functions

```cpp
setMode(String mode)           // Change display mode
setText(String text)           // Set scrolling text
setBrightness(int value)       // Set brightness (0-255)
setScrollSpeed(int speed)      // Set animation speed (10-500ms)
```

### Effect Functions

```cpp
effectRainbow()                // Rainbow cycle
effectScrollText(String text)  // Scrolling text
effectFire()                   // Fire simulation
effectConfetti()              // Confetti sparkles
effectTheaterChase(CRGB color) // Theater chase
effectColorWipe(CRGB color)    // Color wipe
```

## � Documentation

This project includes comprehensive guides:

- **[README.md](README.md)** - Main documentation (this file)
- **[WIRING_GUIDE.md](WIRING_GUIDE.md)** - Complete wiring instructions, power supply options, USB charger triggering
- **[MATRIX_GUIDE.md](MATRIX_GUIDE.md)** - Matrix coordinate system explained with examples
- **[DESIGN.md](DESIGN.md)** - Project architecture and design decisions
- **[config.h](config.h)** - Configuration file with inline comments

## �📄 License

This project is open source and available for personal and educational use.

## 🙏 Credits

- **FastLED Library**: Daniel Garcia and Mark Kriegsman
- **ESP32 Core**: Espressif Systems
- **Community**: Arduino and ESP32 communities

## 💬 Support

For issues, questions, or suggestions:
1. Check the troubleshooting section above
2. Review Serial Monitor output (115200 baud)
3. Verify all hardware connections
4. Check config.h settings

## 🎓 Learning Resources

- [FastLED Documentation](http://fastled.io/)
- [ESP32 Arduino Core](https://docs.espressif.com/projects/arduino-esp32/en/latest/)
- [WS2812B Datasheet](https://cdn-shop.adafruit.com/datasheets/WS2812B.pdf)

---

**Happy coding! 🎉 Light up your world with ESP32! 💡**
