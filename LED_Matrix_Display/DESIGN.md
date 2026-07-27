# LED Matrix Display Controller - Design Document

**Project:** Addressable LED Strip Controller with Wireless Control  
**Platform:** ESP32 (Espressif ESP32-WROOM-32)  
**Version:** 1.0  
**Date:** 2026-07-19  
**Author:** ESP32 LED Matrix Display Project

---

## Table of Contents

1. [Executive Summary](#executive-summary)
2. [System Overview](#system-overview)
3. [Architecture](#architecture)
4. [Hardware Design](#hardware-design)
5. [Software Architecture](#software-architecture)
6. [Communication Protocols](#communication-protocols)
7. [LED Effects Engine](#led-effects-engine)
8. [Web Interface](#web-interface)
9. [Bluetooth Interface](#bluetooth-interface)
10. [Configuration Management](#configuration-management)
11. [Performance Considerations](#performance-considerations)
12. [Security Considerations](#security-considerations)
13. [Future Enhancements](#future-enhancements)
14. [Recent Updates & Lessons Learned](#recent-updates--lessons-learned)
15. [Appendices](#appendices)

---

## Executive Summary

### Project Goals

The LED Matrix Display Controller is an embedded system designed to control addressable RGB LED strips (WS2812B) using an ESP32 microcontroller. The system provides dual wireless control interfaces (WiFi and Bluetooth Low Energy) for real-time manipulation of LED animations, colors, brightness, and text display.

### Key Features

- **Dual Wireless Control**: WiFi web interface and Bluetooth Low Energy (BLE) support
- **Multiple Visual Effects**: 8+ pre-programmed animations including rainbow, fire, confetti, and scrolling text
- **Real-time Control**: Instant response to brightness, speed, and mode changes
- **User-friendly Web Interface**: Responsive HTML5 interface with gradient design
- **Bluetooth UART Protocol**: Nordic UART Service (NUS) compatible BLE implementation
- **Scalable Architecture**: Supports 60-1000+ addressable LEDs
- **Low Latency**: <100ms response time for control commands
- **Power Efficient**: Configurable brightness to manage power consumption

### Target Applications

- Decorative lighting installations
- Event displays and signage
- Art installations
- Smart home ambient lighting
- Wearable LED displays
- Architectural lighting

---

## System Overview

### High-Level Architecture

```
┌─────────────────────────────────────────────────────────────────┐
│                        Control Layer                             │
│  ┌─────────────────┐              ┌──────────────────┐         │
│  │  Web Browser    │              │  BLE Terminal    │         │
│  │  (HTTP Client)  │              │  (NUS Client)    │         │
│  └────────┬────────┘              └────────┬─────────┘         │
│           │ HTTP/HTML                      │ BLE GATT          │
└───────────┼────────────────────────────────┼───────────────────┘
            │                                │
            ▼                                ▼
┌─────────────────────────────────────────────────────────────────┐
│                    Communication Layer                           │
│  ┌─────────────────┐              ┌──────────────────┐         │
│  │  WiFi AP/STA    │              │  BLE Server      │         │
│  │  Web Server     │              │  (NUS Service)   │         │
│  └────────┬────────┘              └────────┬─────────┘         │
└───────────┼────────────────────────────────┼───────────────────┘
            │                                │
            ▼                                ▼
┌─────────────────────────────────────────────────────────────────┐
│                     ESP32 Application Layer                      │
│  ┌───────────────────────────────────────────────────────┐     │
│  │              Control Logic & State Management          │     │
│  │  • Mode Controller                                     │     │
│  │  • Parameter Manager (Brightness, Speed, Text)         │     │
│  │  • Command Parser                                      │     │
│  └────────────────────────┬───────────────────────────────┘     │
│                           │                                      │
│  ┌────────────────────────▼───────────────────────────────┐    │
│  │              LED Effects Engine                         │    │
│  │  • Pattern Generator                                    │    │
│  │  • Animation Controller                                 │    │
│  │  • Text Rendering Engine                                │    │
│  └────────────────────────┬───────────────────────────────┘    │
└───────────────────────────┼──────────────────────────────────────┘
                            │
                            ▼
┌─────────────────────────────────────────────────────────────────┐
│                      Hardware Abstraction Layer                  │
│  ┌───────────────────────────────────────────────────────┐     │
│  │               FastLED Library                          │     │
│  │  • RMT Driver (ESP32 Remote Control)                   │     │
│  │  • Color Space Management                              │     │
│  │  • Hardware PWM Control                                │     │
│  └────────────────────────┬───────────────────────────────┘     │
└───────────────────────────┼──────────────────────────────────────┘
                            │
                            ▼
                   ┌─────────────────┐
                   │  WS2812B Strip  │
                   │  (60+ LEDs)     │
                   └─────────────────┘
```

### System Components

| Component | Technology | Purpose |
|-----------|------------|---------|
| Microcontroller | ESP32-WROOM-32 | Main processing unit |
| LED Driver | FastLED Library | LED control and effects |
| WiFi Stack | ESP32 WiFi (802.11 b/g/n) | Wireless connectivity |
| BLE Stack | ESP32 Bluetooth 4.2 | Bluetooth Low Energy |
| Web Server | ESP32 WebServer Library | HTTP server for web UI |
| LED Strip | WS2812B Addressable RGB | Visual output |

---

## Architecture

### Modular Design

The system follows a modular architecture with clear separation of concerns:

```
LED_Matrix_Display/
├── LED_Matrix_Display.ino    # Main application entry point
├── config.h                   # System configuration
├── wifi_server.h              # WiFi and web server module
├── ble_server.h               # Bluetooth Low Energy module
├── led_effects.h              # LED effects and animations
├── README.md                  # User documentation
├── WIRING_GUIDE.md           # Hardware setup guide
└── DESIGN.md                 # This document
```

### Module Responsibilities

#### Main Application (LED_Matrix_Display.ino)
- System initialization and startup
- Main event loop
- State management
- Module coordination
- Error handling

#### Configuration Module (config.h)
- Hardware parameters (LED count, GPIO pins)
- Network credentials
- Feature flags (WiFi/BLE enable/disable)
- Performance tuning parameters
- Default values

#### WiFi Module (wifi_server.h)
- WiFi Access Point / Station mode management
- HTTP web server
- REST-like API endpoints
- HTML/CSS/JavaScript web interface
- Request parsing and response generation

#### BLE Module (ble_server.h)
- BLE server initialization
- Nordic UART Service (NUS) implementation
- Client connection management
- Command parsing from BLE
- Status notifications

#### LED Effects Module (led_effects.h)
- Animation algorithms
- Color palette management
- Text rendering (5x7 font)
- Scrolling and movement logic
- Pattern generators

---

## Hardware Design

### ESP32 Specifications

**Microcontroller:** ESP32-WROOM-32
- **CPU:** Dual-core Xtensa LX6, 240 MHz
- **Memory:** 520 KB SRAM, 4 MB Flash
- **WiFi:** 802.11 b/g/n (2.4 GHz)
- **Bluetooth:** Bluetooth 4.2 BR/EDR and BLE
- **GPIO:** 34 programmable pins
- **Operating Voltage:** 3.3V logic
- **Supply Voltage:** 5V via USB or VIN pin

### LED Strip Specifications

**Type:** WS2812B Addressable RGB LED
- **Protocol:** Single-wire data transmission
- **Voltage:** 5V DC
- **Current per LED:** ~60mA at full brightness (white)
- **Data Rate:** 800 kHz
- **Color Depth:** 24-bit (8-bit per channel RGB)
- **Update Rate:** 400 Hz typical

### Pin Assignments

| Function | GPIO Pin | Direction | Description |
|----------|----------|-----------|-------------|
| LED Data | GPIO 5 | Output | WS2812B data line |
| USB TX | GPIO 1 | Output | Serial debug output |
| USB RX | GPIO 3 | Input | Serial debug input |
| Built-in LED | GPIO 2 | Output | Status indicator |

### Power Requirements

**Total System Power:**
```
P_total = P_ESP32 + P_LEDs

Where:
- P_ESP32 = 5V × 0.35A = 1.75W (max, WiFi active)
- P_LEDs = 5V × 60 × 0.06A × brightness_factor

Example (60 LEDs at 50% brightness):
P_LEDs = 5V × 60 × 0.06A × 0.5 = 9W
P_total = 1.75W + 9W = 10.75W (~2.15A at 5V)
```

**Recommended Power Supply:** 5V 4-5A regulated DC power supply

### Signal Conditioning

**Data Line:**
- 470Ω series resistor for protection
- Optional 3.3V → 5V level shifter (though usually works without)
- Keep data line < 2 meters for reliability

**Power:**
- 1000µF capacitor across power rails (near LED strip)
- Separate power and data ground connections
- Star ground topology recommended

---

## Software Architecture

### State Machine

```
                    ┌─────────────┐
                    │   STARTUP   │
                    └──────┬──────┘
                           │
                           ▼
                    ┌─────────────┐
          ┌────────►│    INIT     │
          │         └──────┬──────┘
          │                │
          │                ▼
          │         ┌─────────────┐
          │         │   RUNNING   │◄────┐
          │         └──────┬──────┘     │
          │                │             │
          │                │ Command     │ Update
          │                ▼             │
          │         ┌─────────────┐     │
          │         │  PROCESSING │─────┘
          │         └──────┬──────┘
          │                │
          │                │ Error
          │                ▼
          │         ┌─────────────┐
          └─────────│    ERROR    │
                    └─────────────┘
```

### Main Loop Flow

```cpp
void loop() {
    // 1. Handle incoming commands
    if (ENABLE_WIFI) {
        webServer.handleClient();  // Process HTTP requests
    }
    
    if (ENABLE_BLUETOOTH) {
        handleBLE();  // Process BLE notifications
    }
    
    // 2. Update LED display
    updateDisplay();  // Render current effect
    
    // 3. Small delay for stability
    delay(10);
}
```

### Threading Model

ESP32 dual-core usage:
- **Core 0 (Protocol Core):** WiFi, BLE, networking tasks
- **Core 1 (Application Core):** Main loop, LED updates, effects rendering

FreeRTOS tasks run automatically for WiFi/BLE stack.

### Memory Management

**RAM Allocation:**
```cpp
// LED array: 60 LEDs × 3 bytes/LED = 180 bytes
CRGB leds[NUM_LEDS];

// Web server buffers: ~8KB
// BLE stack: ~30KB
// WiFi stack: ~40KB
// Application: ~10KB

Total RAM usage: ~88KB / 520KB available
```

**Flash Usage:**
- Program code: ~2MB
- OTA partition: ~3MB (huge_app partition scheme)
- Filesystem: Not used

---

## Communication Protocols

### WiFi Communication

#### Access Point Mode
```
Network: LED_Matrix_AP
Password: 12345678
IP Address: 192.168.4.1
Subnet: 255.255.255.0
DHCP Range: 192.168.4.2 - 192.168.4.10
```

#### HTTP API Endpoints

| Endpoint | Method | Parameters | Response | Description |
|----------|--------|------------|----------|-------------|
| `/` | GET | - | HTML | Main web interface |
| `/setMode` | GET | `mode` | OK/Error | Change display mode |
| `/setText` | GET | `text` | OK/Error | Update scrolling text |
| `/setBrightness` | GET | `value` | OK/Error | Set brightness (0-255) |
| `/setSpeed` | GET | `value` | OK/Error | Set speed (10-500ms) |
| `/status` | GET | - | JSON | Get current state |

**Example API Calls:**
```http
GET /setMode?mode=rainbow
GET /setText?text=Hello%20World
GET /setBrightness?value=200
GET /setSpeed?value=75
GET /status
```

**Status Response Format:**
```json
{
  "mode": "rainbow",
  "text": "Hello ESP32!",
  "brightness": 128,
  "speed": 50
}
```

### Bluetooth Low Energy

#### Service Architecture

**Nordic UART Service (NUS) UUIDs:**
```
Service UUID:         6E400001-B5A3-F393-E0A9-E50E24DCCA9E
RX Characteristic:    6E400002-B5A3-F393-E0A9-E50E24DCCA9E (Write)
TX Characteristic:    6E400003-B5A3-F393-E0A9-E50E24DCCA9E (Notify)
```

#### BLE Command Protocol

**Command Format:**
```
CMD:VALUE
```

**Supported Commands:**

| Command | Value Format | Example | Description |
|---------|-------------|---------|-------------|
| MODE | mode_name | `MODE:rainbow` | Change effect mode |
| TEXT | string | `TEXT:Hello World` | Set scrolling text |
| BRIGHTNESS | 0-255 | `BRIGHTNESS:200` | Set LED brightness |
| SPEED | 10-500 | `SPEED:100` | Set animation speed (ms) |
| COLOR | R,G,B | `COLOR:255,0,0` | Set custom color |

**Status Notifications:**

Sent every 5 seconds when connected:
```
STATUS:mode=rainbow,brightness=128,speed=50
```

#### Connection Flow

```
Client                          ESP32 Server
  │                                  │
  │──── Scan for devices ───────────►│
  │                                  │
  │◄─── Advertisement (LED_Matrix) ──│
  │                                  │
  │──── Connect ────────────────────►│
  │                                  │
  │◄─── Connection Established ──────│
  │                                  │
  │──── Discover Services ──────────►│
  │                                  │
  │◄─── NUS Service (6E40...) ───────│
  │                                  │
  │──── Write to RX Char ───────────►│
  │     "MODE:fire"                  │
  │                                  │
  │◄─── Notify from TX Char ─────────│
  │     "STATUS:mode=fire..."        │
  │                                  │
```

---

## LED Effects Engine

### Effect Architecture

Each effect is a self-contained function that updates the LED array:

```cpp
void effectName() {
    // 1. Update internal state (position, hue, etc.)
    // 2. Calculate colors for each LED
    // 3. Write to leds[] array
    // 4. FastLED.show() called by main loop
}
```

### Available Effects

#### 1. Rainbow Cycle
**Algorithm:** Fill strip with rainbow colors, shift hue over time
```cpp
void effectRainbow() {
    fill_rainbow(leds, NUM_LEDS, gHue, 7);
    gHue++;
}
```
**Performance:** O(n) where n = LED count

#### 2. Fire Simulation
**Algorithm:** Heat diffusion model with random sparks
```cpp
void effectFire() {
    // Cool down each cell
    // Drift heat upward
    // Randomly ignite new sparks
    // Map heat to color palette
}
```
**Performance:** O(n), updates at ~60 FPS

#### 3. Confetti
**Algorithm:** Random colored sparkles on fading background
```cpp
void effectConfetti() {
    fadeToBlackBy(leds, NUM_LEDS, 10);
    int pos = random16(NUM_LEDS);
    leds[pos] += CHSV(gHue + random8(64), 200, 255);
}
```
**Performance:** O(n) fade + O(1) sparkle

#### 4. Scrolling Text
**Algorithm:** Render text using 5x7 font, scroll position over time
```cpp
void effectScrollText(String text) {
    // Clear display
    // For each character:
    //   Get font data
    //   Render at scroll position
    // Increment scroll position
}
```
**Performance:** O(text_length × font_width)

#### 5. Theater Chase
**Algorithm:** Moving dots with gaps, classic marquee effect
```cpp
void effectTheaterChase(CRGB color) {
    // Every 3rd LED on
    // Shift pattern each frame
}
```
**Performance:** O(n)

#### 6. Color Wipe
**Algorithm:** Fill strip sequentially with color
```cpp
void effectColorWipe(CRGB color) {
    static int pos = 0;
    leds[pos] = color;
    pos = (pos + 1) % NUM_LEDS;
}
```
**Performance:** O(1) per frame

### Text Rendering Engine

**Font Format:** 5x7 pixel font stored as byte arrays
```cpp
const byte font5x7[][5] = {
    {0x3E, 0x51, 0x49, 0x45, 0x3E}, // '0'
    {0x00, 0x42, 0x7F, 0x40, 0x00}, // '1'
    // ... more characters
};
```

**Character Set:**
- Digits: 0-9
- Uppercase: A-Z
- Lowercase: a-z (mapped to uppercase)
- Space character

**Scrolling Logic:**
```cpp
scrollPosition--;  // Move left
if (scrollPosition < -textWidth) {
    scrollPosition = NUM_LEDS;  // Reset to right
}
```

### Color Management

**Color Spaces:**
- RGB: Direct red, green, blue values (0-255 each)
- HSV: Hue (0-255), Saturation (0-255), Value/Brightness (0-255)

**FastLED Color Types:**
```cpp
CRGB color1 = CRGB::Red;           // RGB
CRGB color2 = CRGB(255, 128, 0);   // RGB values
CHSV color3 = CHSV(160, 255, 255); // HSV (blue-green)
```

---

## Web Interface

### Technology Stack

- **HTML5:** Structure and semantic markup
- **CSS3:** Styling with gradients, flexbox, animations
- **JavaScript (ES6):** Client-side logic and AJAX
- **Fetch API:** HTTP requests to ESP32

### UI Design Principles

1. **Mobile-First:** Responsive design for phone/tablet screens
2. **Visual Feedback:** Status messages, button animations
3. **Intuitive Controls:** Sliders, dropdowns, clear labels
4. **Real-time Updates:** Instant response to user input
5. **Modern Aesthetics:** Gradient backgrounds, rounded corners

### Component Layout

```
┌─────────────────────────────────────┐
│     🎨 LED Matrix Display          │ ← Header
├─────────────────────────────────────┤
│  Display Mode                       │
│  [Dropdown: Rainbow ▼]              │ ← Mode Selector
├─────────────────────────────────────┤
│  Scrolling Text                     │
│  [Input: Enter text...]             │
│  [Update Text Button]               │ ← Text Input
├─────────────────────────────────────┤
│  Brightness: 128                    │
│  [────●─────────────] 0-255         │ ← Brightness Slider
├─────────────────────────────────────┤
│  Animation Speed: 50ms              │
│  [─────●────────────] 10-500        │ ← Speed Slider
├─────────────────────────────────────┤
│         Ready                       │ ← Status Area
└─────────────────────────────────────┘
```

### JavaScript Functions

```javascript
// Update display mode
function setMode() {
    let mode = document.getElementById('mode').value;
    fetch('/setMode?mode=' + mode);
}

// Update brightness
function setBrightness() {
    let brightness = document.getElementById('brightness').value;
    fetch('/setBrightness?value=' + brightness);
}

// Update text
function setText() {
    let text = document.getElementById('text').value;
    fetch('/setText?text=' + encodeURIComponent(text));
}
```

### Styling

**Color Scheme:**
- Background gradient: `#667eea` → `#764ba2` (purple/blue)
- Controls background: Semi-transparent white (`rgba(255,255,255,0.1)`)
- Accent color: `#f093fb` → `#f5576c` (pink gradient)
- Text: White on dark, dark on light

**Responsive Breakpoints:**
- Mobile: < 600px (full width)
- Tablet: 600px - 1024px (centered, max-width: 600px)
- Desktop: > 1024px (centered, max-width: 600px)

---

## Bluetooth Interface

### BLE Stack Configuration

```cpp
BLEDevice::init(BLE_DEVICE_NAME);  // Initialize
BLEServer *pServer = BLEDevice::createServer();
BLEService *pService = pServer->createService(SERVICE_UUID);
```

### Characteristic Properties

**RX Characteristic (Write):**
- Property: `PROPERTY_WRITE` + `PROPERTY_WRITE_NR`
- Permission: Write without response
- Max size: 512 bytes
- Used for: Receiving commands from client

**TX Characteristic (Notify):**
- Property: `PROPERTY_READ` + `PROPERTY_NOTIFY`
- Permission: Read + Notify
- Max size: 512 bytes
- Used for: Sending status to client

### Connection Management

```cpp
class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
        deviceConnected = true;
        Serial.println("BLE Client connected");
    }
    
    void onDisconnect(BLEServer* pServer) {
        deviceConnected = false;
        // Restart advertising for reconnection
        BLEDevice::startAdvertising();
    }
};
```

### Command Parsing

```cpp
void onWrite(BLECharacteristic *pCharacteristic) {
    String command = pCharacteristic->getValue().c_str();
    
    // Parse "CMD:VALUE" format
    int colonIndex = command.indexOf(':');
    String cmd = command.substring(0, colonIndex);
    String val = command.substring(colonIndex + 1);
    
    // Execute command
    if (cmd == "MODE") setMode(val);
    else if (cmd == "TEXT") setText(val);
    // ... etc
}
```

---

## Configuration Management

### Configuration File Structure

```cpp
// Hardware configuration
#define NUM_LEDS 60
#define LED_PIN 5
#define LED_TYPE WS2812B
#define COLOR_ORDER GRB

// Network configuration
#define WIFI_SSID "NetworkName"
#define WIFI_PASSWORD "Password"
#define AP_MODE false

// Feature flags
#define ENABLE_WIFI true
#define ENABLE_BLUETOOTH true

// Performance tuning
#define FRAMES_PER_SECOND 60
#define WIFI_TIMEOUT 10000
```

### Compile-Time Configuration

All settings are `#define` macros, allowing:
- Zero runtime overhead
- Compiler optimizations
- Conditional compilation
- Easy customization

### Runtime State

```cpp
// Current operating parameters
String currentMode = "rainbow";
String currentText = "Hello ESP32!";
int brightness = 128;
int scrollSpeed = 50;
bool deviceConnected = false;
```

---

## Performance Considerations

### CPU Usage

**Typical Load Distribution:**
- LED updates: 40-50%
- WiFi/BLE stack: 20-30%
- Web server: 10-15%
- Effects computation: 15-20%
- Idle: 5-10%

### Memory Usage

**RAM:**
- Total available: 520 KB
- Used: ~88 KB (17%)
- Free: ~432 KB (83%)

**Flash:**
- Program: ~2 MB
- Free for updates: ~1.1 MB

### Update Rates

| Component | Rate | Latency |
|-----------|------|---------|
| LED refresh | 60 FPS | 16.7 ms |
| Effect update | 20-60 FPS | 17-50 ms |
| Web request | On-demand | <100 ms |
| BLE notify | 5s intervals | N/A |

### Bottlenecks

1. **WS2812B Protocol:** 800 kHz bit rate limits update speed
2. **WiFi Stack:** Can cause LED timing jitter (mitigated by FastLED)
3. **Memory Bandwidth:** Large LED counts (1000+) may slow updates

### Optimization Techniques

1. **Hardware SPI/RMT:** FastLED uses ESP32 RMT peripheral for precise timing
2. **DMA Transfers:** Non-blocking LED updates
3. **Minimal Allocations:** Static arrays, no dynamic memory
4. **Efficient Algorithms:** O(n) or better for all effects
5. **Dual Core:** Networking on Core 0, LEDs on Core 1

---

## Security Considerations

### Current Implementation

**Security Level:** Basic/None (suitable for home use)

**Access Control:**
- WiFi: WPA2 password (if configured)
- Access Point: Password protected
- BLE: No pairing required
- HTTP: No authentication

### Vulnerabilities

1. **No Authentication:** Anyone can connect and control
2. **No Encryption:** Commands sent in plain text
3. **No Input Validation:** Limited sanitization
4. **No Rate Limiting:** Potential for command flooding
5. **Default Credentials:** Well-known AP password

### Recommendations for Production

1. **Add HTTP Authentication:**
   ```cpp
   if (!webServer.authenticate("admin", "password")) {
       return webServer.requestAuthentication();
   }
   ```

2. **Implement BLE Pairing:**
   ```cpp
   BLESecurity *pSecurity = new BLESecurity();
   pSecurity->setAuthenticationMode(ESP_LE_AUTH_REQ_SC_MITM_BOND);
   ```

3. **Input Validation:**
   - Sanitize text input
   - Validate numeric ranges
   - Prevent buffer overflows

4. **HTTPS/TLS:**
   - Use HTTPS for web interface
   - Requires SSL certificate

5. **Firmware Updates:**
   - Implement secure OTA updates
   - Sign firmware images

---

## Future Enhancements

### Phase 1: Core Improvements

1. **More Effects:**
   - Wave patterns
   - Audio reactive (with microphone)
   - Matrix-style effects
   - Custom pattern designer

2. **Advanced Text:**
   - Multiple fonts
   - Variable text sizes
   - Text colors and styles
   - Graphics/icons

3. **Scheduling:**
   - Time-based automation
   - Sunrise/sunset scheduling
   - Event triggers

### Phase 2: Connectivity

1. **MQTT Integration:**
   - Home Assistant integration
   - IoT platform support
   - Remote control via cloud

2. **Voice Control:**
   - Alexa integration
   - Google Assistant support

3. **Mobile App:**
   - Native iOS/Android app
   - Advanced color picker
   - Pattern editor

### Phase 3: Hardware Extensions

1. **Multi-Strip Support:**
   - Control multiple strips independently
   - Synchronized animations

2. **Sensor Integration:**
   - Motion detection
   - Ambient light sensor
   - Temperature monitoring

3. **Power Monitoring:**
   - Current sensing
   - Over-current protection
   - Battery level indication

### Phase 4: Advanced Features

1. **Pattern Storage:**
   - Save custom patterns to SPIFFS
   - Pattern sharing/download
   - User-created effects

2. **Music Sync:**
   - Beat detection
   - Frequency analysis
   - Multiple sync modes

3. **AI Integration:**
   - Mood-based lighting
   - Adaptive brightness
   - Learning user preferences

---

## Recent Updates & Lessons Learned

### Version 1.1 Updates (July 2026)

#### 🆕 New Features

**1. Web-Based LED Configuration**

Added dynamic LED configuration through the web interface:
- Matrix dimensions (width × height) configuration
- Total LED count automatically calculated (width × height)
- Settings persist using ESP32 Preferences library
- Changes take effect after ESP32 restart

**Implementation:**
```cpp
// New endpoint in wifi_server.h
/setMatrixSize?width=W&height=H  - Set matrix dimensions

// LED count automatically calculated
int totalLEDs = width * height;

// All stored in NVS (Non-Volatile Storage)
Preferences prefs;
prefs.begin("led-config", false);
prefs.putInt("num_leds", totalLEDs);  // Calculated value
prefs.putInt("matrix_w", width);
prefs.putInt("matrix_h", height);
```

**Benefits:**
- No code recompilation needed for configuration changes
- Prevents mismatch between LED count and matrix dimensions
- Single source of truth (width and height define everything)
- Easier for non-technical users
- Portable configuration across different installations

**2. Comprehensive Documentation**

Added three new documentation files:
- **MATRIX_GUIDE.md** - Complete guide to matrix coordinate systems
- Updated **WIRING_GUIDE.md** - USB charger triggering information
- Enhanced **README.md** - New configuration features

#### 🔧 Hardware Lessons Learned

**1. Data Line Resistor Issues**

**Problem:** Adding 470Ω resistor to data line prevented LEDs from working.

**Root Cause:**
- WS2812B LEDs require clean 3.3V-5V logic signal
- 470Ω resistor creates voltage drop that weakens signal
- ESP32 GPIO outputs ~3.3V; resistor drops this below WS2812B threshold
- Short wire runs (< 1m) don't need additional resistance

**Solution Implemented:**
- Removed data line resistor for short connections
- Updated documentation to make resistor optional
- Added alternative: logic level shifter for long runs (> 1m)

**New Recommendations:**
```
Direct connection (< 1m):    ESP32 GPIO 14 ──► LED DI
With level shifter (> 1m):   ESP32 GPIO 14 ──► Shifter ──► LED DI
With resistor (alternative): ESP32 GPIO 14 ──[470Ω]──► LED DI (may not work)
```

**2. USB Charger Power Supply Issues**

**Problem:** USB charger showed 0V output, wouldn't power LEDs.

**Root Cause:**
Modern USB chargers (especially USB-C) require load detection or CC pin configuration:
- **USB-A chargers**: Usually auto-trigger but some need small load
- **USB-C chargers**: Require 5.1kΩ pull-down resistors on CC1/CC2 pins
- **USB-PD chargers**: Need power negotiation protocol
- **Quick Charge**: Needs specific voltage divider on D+/D- pins

**Solutions Documented:**

1. **For USB-C Power Delivery:**
   ```
   USB-C CC1 pin ──[5.1kΩ]── GND
   USB-C CC2 pin ──[5.1kΩ]── GND
   ```

2. **Easier alternatives:**
   - Use USB-C breakout board (includes resistors)
   - Use USB-C to USB-A adapter
   - Use dedicated 5V power supply with screw terminals

3. **For USB-A chargers:**
   - Usually work automatically
   - If not: temporarily connect 100Ω between +5V and GND to trigger

**Power Supply Recommendations Updated:**

| Source | Best For | Pros | Cons |
|--------|----------|------|------|
| Dedicated 5V supply | Production | Reliable, high current | More expensive |
| USB-A charger | Testing | Convenient | Limited current |
| USB-C PD | Portable | Compact | Needs triggering circuit |
| Computer PSU | Large displays | Very high current | Bulky |

#### 📐 Matrix Coordinate System Clarification

**Problem:** Users confused about how LED indices map to (row, column) coordinates.

**Documentation Added:**

1. **Standard Formula:**
   ```cpp
   // For zigzag wiring (most common):
   ledIndex = (row % 2 == 0) ?
              row * width + col :              // Even rows: left to right
              row * width + (width - 1 - col); // Odd rows: right to left
   ```

2. **Visual Examples:**
   - 10×6 matrix layout diagrams
   - LED index mapping tables
   - Code examples for drawing shapes

3. **Helper Functions:**
   ```cpp
   void setPixel(int x, int y, CRGB color);
   void drawHLine(int y, int x1, int x2, CRGB color);
   void drawVLine(int x, int y1, int y2, CRGB color);
   void drawRect(int x1, int y1, int x2, int y2, CRGB color);
   ```

#### 🎯 Configuration Best Practices

**GPIO Pin Selection:**
- Changed default from GPIO 5 to GPIO 14
- Reason: GPIO 5 has pull-up resistor that can interfere with boot
- GPIO 14 is "boot-safe" (no boot-time functions)

**Partition Scheme:**
- Using "Huge APP (3MB No OTA)" for maximum program space
- Allows complex effects and large web interfaces
- Trade-off: No OTA updates (acceptable for wired programming)

**Memory Management:**
```cpp
// Static allocation for predictable behavior
CRGB leds[NUM_LEDS];  // Maximum 1000 LEDs = 3KB

// Dynamic configuration stored in NVS
int active_leds = NUM_LEDS;  // Can be less than array size
```

#### 🔒 Security Considerations Added

**Web Interface:**
- Currently no authentication (acceptable for private networks)
- Future: Add HTTP Basic Auth or token-based auth
- Recommendation: Use WPA2 with strong password for AP mode

**BLE Security:**
- No pairing required (convenience over security)
- Acceptable for low-risk applications
- Consider adding pairing for public installations

#### 📊 Testing Results

**Configuration:**
- 60 WS2812B LEDs
- ESP32-DevKitC V4
- 5V 5A power supply
- No data line resistor
- Wire length: ~15cm

**Performance:**
- Web response time: < 100ms
- BLE command latency: < 50ms
- Effect update rate: 60 FPS
- WiFi connection time: 2-3 seconds
- BLE connection time: 1-2 seconds

**Power Measurements:**
```
Idle (LEDs off):        0.25A @ 5V = 1.25W
Rainbow 50% brightness: 1.8A @ 5V = 9W
Full white 100%:        3.6A @ 5V = 18W
```

#### 🐛 Known Issues & Workarounds

**1. ESP32 Brownout on LED Power-On**
- **Symptom:** ESP32 resets when many LEDs light up suddenly
- **Cause:** Inrush current drops voltage
- **Workaround:** Fade in brightness gradually, use larger capacitor (2200µF)

**2. WiFi Disconnects During Heavy LED Updates**
- **Symptom:** Web interface becomes unresponsive during fire effect
- **Cause:** LED updates monopolize CPU
- **Workaround:** Reduce FPS, use dual-core task scheduling

**3. First LED Sometimes Shows Wrong Color**
- **Symptom:** LED[0] flickers or shows incorrect color
- **Cause:** Signal reflection, insufficient settling time
- **Workaround:** Add small delay after FastLED.show(), or use LED[0] as buffer

#### 📈 Future Configuration Enhancements

**Planned:**
1. Web-based effect parameter tuning
2. Color palette customization
3. Pattern presets save/load
4. Network settings via web interface
5. Firmware update via web (OTA)

**Under Consideration:**
1. Mobile app for configuration
2. Cloud backup of settings
3. Multi-device synchronization
4. Voice command integration

---

## Appendices

### A. Effect Mode Reference

| Mode | Description | CPU Usage | Power Draw |
|------|-------------|-----------|------------|
| `rainbow` | Cycling rainbow colors | Low | Medium |
| `scroll_text` | Scrolling text display | Medium | Medium |
| `color_wipe` | Sequential color fill | Low | Variable |
| `theater_chase` | Marquee chasing lights | Low | Medium |
| `fire` | Fire simulation | Medium | Medium |
| `confetti` | Random sparkles | Low | Low-Medium |
| `solid` | Solid white light | Low | High |
| `off` | All LEDs off | Minimal | Minimal |

### B. GPIO Pin Map (ESP32-WROOM-32)

```
                    ESP32
         ┌────────────────────┐
         │                    │
    3V3  │ 1            30 │ GND
    EN   │ 2            29 │ GPIO23
    SVP  │ 3            28 │ GPIO22
    SVN  │ 4            27 │ GPIO1 (TX)
  GPIO34 │ 5            26 │ GPIO3 (RX)
  GPIO35 │ 6            25 │ GPIO21
  GPIO32 │ 7            24 │ GND
  GPIO33 │ 8            23 │ GPIO19
  GPIO25 │ 9            22 │ GPIO18
  GPIO26 │10            21 │ GPIO5 ★ LED Data
  GPIO27 │11            20 │ GPIO17
  GPIO14 │12            19 │ GPIO16
  GPIO12 │13            18 │ GPIO4
    GND  │14            17 │ GPIO2 (LED)
  GPIO13 │15            16 │ GPIO15
         │                    │
         └────────────────────┘
```

★ = Used in this project

### C. Power Calculation Examples

**Example 1: 60 LEDs, Rainbow mode, 75% brightness**
```
I_LED = 60 × 0.06A × 0.75 × 0.5 (rainbow uses ~50% power)
      = 1.35A

I_ESP32 = 0.25A (WiFi active)

I_total = 1.35A + 0.25A = 1.6A
P_total = 5V × 1.6A = 8W

Recommended supply: 5V 2.5A minimum
```

**Example 2: 100 LEDs, Full white, 100% brightness**
```
I_LED = 100 × 0.06A × 1.0 = 6A
I_ESP32 = 0.25A
I_total = 6.25A
P_total = 31.25W

Recommended supply: 5V 8A with power injection
```

### D. Timing Specifications

**WS2812B Protocol Timing:**
```
Bit 0: HIGH 0.4µs, LOW 0.85µs
Bit 1: HIGH 0.8µs, LOW 0.45µs
Reset: LOW > 50µs

Data rate: 800 kHz
Color order: GRB (Green, Red, Blue)
Bit depth: 24-bit (8 bits per channel)
```

**Update Timing:**
```
60 LEDs × 24 bits/LED × 1.25µs/bit = 1.8ms per update
Maximum theoretical FPS: 555 Hz
Practical FPS with overhead: 60-120 Hz
```

### E. BLE Advertisement Packet

```
Flags: 0x06 (LE General Discoverable, BR/EDR Not Supported)
Complete Local Name: "LED_Matrix_Display"
Service UUIDs: 6E400001-B5A3-F393-E0A9-E50E24DCCA9E
TX Power Level: 0 dBm
```

### F. Web Interface HTML Structure

```html
<!DOCTYPE html>
<html>
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>LED Matrix Display</title>
  <style>/* Embedded CSS */</style>
</head>
<body>
  <div class="container">
    <h1>LED Matrix Display</h1>
    <div class="control-group"><!-- Mode selector --></div>
    <div class="control-group"><!-- Text input --></div>
    <div class="control-group"><!-- Brightness slider --></div>
    <div class="control-group"><!-- Speed slider --></div>
    <div class="status">Ready</div>
  </div>
  <script>/* Embedded JavaScript */</script>
</body>
</html>
```

### G. Library Dependencies

```
FastLED v3.10.5+
- Hardware abstraction for LED control
- Color space conversions
- Effects helpers

WiFi (ESP32 Core)
- WiFi connectivity
- AP and Station modes

WebServer (ESP32 Core)
- HTTP server
- Request routing

BLE (ESP32 Core)
- Bluetooth Low Energy stack
- GATT server implementation
```

### H. Build Configuration

**Arduino IDE / arduino-cli:**
```
Board: ESP32 Dev Module
Partition Scheme: Huge APP (3MB No OTA/1MB SPIFFS)
Flash Size: 4MB
Upload Speed: 921600
Core Debug Level: None (for production)
```

### I. Troubleshooting Guide

| Issue | Possible Cause | Solution |
|-------|----------------|----------|
| LEDs not lighting | Wrong GPIO pin | Check config.h LED_PIN |
| Random colors | Insufficient power | Use adequate power supply |
| WiFi not connecting | Wrong credentials | Update WIFI_SSID/PASSWORD |
| BLE not visible | Device already connected | Reset ESP32 |
| Web page not loading | IP address changed | Check Serial Monitor |
| Flickering | Voltage drop | Add capacitor, thicker wires |

---

## Document Revision History

| Version | Date | Author | Changes |
|---------|------|--------|---------|
| 1.0 | 2026-07-19 | Initial | Complete design document |
| 1.1 | 2026-07-22 | Update | Added web-based LED configuration, hardware lessons learned, USB charger triggering, matrix coordinate documentation |

---

**End of Design Document**
