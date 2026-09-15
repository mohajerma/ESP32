# BT Audio Receiver Library for ESP32

A simple, easy-to-use Arduino library for receiving Bluetooth audio streams on ESP32 boards using the A2DP (Advanced Audio Distribution Profile) protocol. Turn your ESP32 into a Bluetooth speaker with just a few lines of code!

## 🎵 Features

- **Simple API**: Get started with just 3 lines of code
- **A2DP Sink Protocol**: Industry-standard Bluetooth audio streaming
- **Internal DAC Support**: Use ESP32's built-in DAC (GPIO25/GPIO26)
- **External I2S DAC Support**: Connect high-quality I2S DACs for better audio
- **Flexible Configuration**: Customizable device name, pins, sample rate
- **Event Callbacks**: Monitor connection state and audio playback
- **Volume Control**: Adjust audio volume programmatically
- **Low Latency**: Optimized for real-time audio streaming
- **Well Documented**: Multiple examples and detailed documentation

## 📦 Installation

### Arduino IDE

1. Download this library as a ZIP file
2. In Arduino IDE: **Sketch** → **Include Library** → **Add .ZIP Library**
3. Select the downloaded ZIP file
4. Restart Arduino IDE

### Manual Installation

1. Download or clone this repository
2. Copy the `BT_Audio_Library` folder to your Arduino libraries directory:
   - **Windows**: `Documents\Arduino\libraries\`
   - **Mac**: `~/Documents/Arduino/libraries/`
   - **Linux**: `~/Arduino/libraries/`
3. Restart Arduino IDE

## 🚀 Quick Start

### Basic Example (Internal DAC)

```cpp
#include <BT_Audio_Receiver.h>

BT_Audio_Receiver btAudio;

void setup() {
  Serial.begin(115200);
  
  // Initialize with device name
  btAudio.begin("ESP32_Speaker");
  
  // Use internal DAC (GPIO25 and GPIO26)
  btAudio.useInternalDAC();
  
  Serial.println("Bluetooth Speaker Ready!");
}

void loop() {
  // Audio is streamed automatically
  delay(1000);
}
```

That's it! Your ESP32 is now discoverable as "ESP32_Speaker" and ready to receive audio.

## 🔌 Hardware Setup

### Option 1: Internal DAC (Simplest)

The ESP32 has a built-in 8-bit DAC on GPIO25 and GPIO26:

```
ESP32 GPIO25 (DAC1) → Right Channel → Speaker/Amplifier
ESP32 GPIO26 (DAC2) → Left Channel  → Speaker/Amplifier
```

**Note**: Internal DAC provides basic audio quality. For better quality, use an external I2S DAC.

#### Simple RC Filter (Optional but Recommended)

Add a simple RC low-pass filter to smooth the DAC output:

```
GPIO25 → 1kΩ resistor → 10µF capacitor to GND → Right Channel Output
GPIO26 → 1kΩ resistor → 10µF capacitor to GND → Left Channel Output
```

### Option 2: External I2S DAC (Better Quality)

For high-quality audio, use an external I2S DAC like PCM5102, MAX98357A, or similar:

#### Default Wiring (PCM5102 Example)

```
ESP32      I2S DAC
GPIO26  →  BCK  (Bit Clock)
GPIO25  →  WS   (Word Select / LRC)
GPIO22  →  DATA (Data In)
GND     →  GND
3.3V    →  VIN
```

#### PCM5102 Additional Connections

```
SCK  → GND (enables auto-detect mode)
FLT  → GND (normal filter mode)
DEMP → GND (de-emphasis off)
FMT  → GND (I2S format)
XMT  → 3.3V or leave floating (soft mute off)
```

#### Code for I2S DAC

```cpp
#include <BT_Audio_Receiver.h>

BT_Audio_Receiver btAudio;

void setup() {
  Serial.begin(115200);
  btAudio.begin("ESP32_HiFi_Speaker");
  
  // Configure I2S pins and sample rate
  btAudio.setupI2S(26, 25, 22, 44100);  // BCK, WS, DATA, Sample Rate
  
  Serial.println("I2S Bluetooth Speaker Ready!");
}

void loop() {
  delay(1000);
}
```

## 📚 API Reference

### Initialization

#### `begin(deviceName)`
Initialize the Bluetooth audio receiver.

```cpp
bool begin(const char* deviceName = "ESP32_BT_Speaker")
```

**Parameters:**
- `deviceName`: Name visible to other Bluetooth devices (default: "ESP32_BT_Speaker")

**Returns:** `true` if successful, `false` otherwise

**Example:**
```cpp
btAudio.begin("My ESP32 Speaker");
```

---

#### `end()`
Stop the Bluetooth audio receiver and release resources.

```cpp
void end()
```

---

### Audio Output Configuration

#### `useInternalDAC()`
Configure the receiver to use ESP32's internal DAC on GPIO25 and GPIO26.

```cpp
bool useInternalDAC()
```

**Returns:** `true` if successful, `false` otherwise

**Example:**
```cpp
btAudio.useInternalDAC();
```

---

#### `setupI2S(bckPin, wsPin, dataPin, sampleRate)`
Configure the receiver to use an external I2S DAC.

```cpp
bool setupI2S(int bckPin = 26, int wsPin = 25, int dataPin = 22, int sampleRate = 44100)
```

**Parameters:**
- `bckPin`: Bit clock pin (default: 26)
- `wsPin`: Word select pin (default: 25)
- `dataPin`: Data output pin (default: 22)
- `sampleRate`: Audio sample rate in Hz (default: 44100)

**Returns:** `true` if successful, `false` otherwise

**Example:**
```cpp
btAudio.setupI2S(26, 25, 22, 44100);  // Standard setup
btAudio.setupI2S(14, 15, 27, 48000);  // Custom pins and 48kHz
```

---

### Device Information

#### `setDeviceName(name)`
Set the Bluetooth device name (must be called before `begin()`).

```cpp
void setDeviceName(const char* name)
```

**Example:**
```cpp
btAudio.setDeviceName("Living Room Speaker");
btAudio.begin();  // Name is applied here
```

---

#### `getDeviceName()`
Get the current device name.

```cpp
const char* getDeviceName()
```

**Returns:** Current device name string

---

#### `getBluetoothAddress()`
Get the Bluetooth MAC address of the ESP32.

```cpp
String getBluetoothAddress()
```

**Returns:** MAC address as a formatted string (e.g., "AA:BB:CC:DD:EE:FF")

**Example:**
```cpp
Serial.println("MAC: " + btAudio.getBluetoothAddress());
```

---

### Connection Status

#### `isConnected()`
Check if a device is currently connected.

```cpp
bool isConnected()
```

**Returns:** `true` if connected, `false` otherwise

**Example:**
```cpp
if (btAudio.isConnected()) {
  Serial.println("Device is connected");
}
```

---

#### `disconnect()`
Disconnect the currently connected device.

```cpp
void disconnect()
```

---

### Volume Control

#### `setVolume(volume)`
Set the audio volume.

```cpp
void setVolume(uint8_t volume)
```

**Parameters:**
- `volume`: Volume level (0-127)

**Example:**
```cpp
btAudio.setVolume(100);  // ~80% volume
btAudio.setVolume(127);  // Maximum volume
```

---

#### `getVolume()`
Get the current volume level.

```cpp
uint8_t getVolume()
```

**Returns:** Current volume (0-127)

---

### Event Callbacks

#### `onConnectionStateChanged(callback)`
Register a callback for connection state changes.

```cpp
void onConnectionStateChanged(bt_connection_state_callback_t callback)
```

**Example:**
```cpp
btAudio.onConnectionStateChanged([](esp_a2d_connection_state_t state, void* param) {
  if (state == ESP_A2D_CONNECTION_STATE_CONNECTED) {
    Serial.println("Connected!");
  } else {
    Serial.println("Disconnected!");
  }
});
```

---

#### `onAudioStateChanged(callback)`
Register a callback for audio playback state changes.

```cpp
void onAudioStateChanged(bt_audio_state_callback_t callback)
```

**Example:**
```cpp
btAudio.onAudioStateChanged([](esp_a2d_audio_state_t state, void* param) {
  if (state == ESP_A2D_AUDIO_STATE_STARTED) {
    Serial.println("Playing");
  } else if (state == ESP_A2D_AUDIO_STATE_STOPPED) {
    Serial.println("Stopped");
  }
});
```

---

#### `onDataReceived(callback)`
Register a callback that receives raw audio data.

```cpp
void onDataReceived(bt_audio_data_callback_t callback)
```

**Example:**
```cpp
btAudio.onDataReceived([](const uint8_t *data, uint32_t len) {
  // Process audio data here
  Serial.printf("Received %d bytes\n", len);
});
```

## 📋 Examples

The library includes three complete examples:

### 1. BasicReceiver
Simple Bluetooth speaker using internal DAC. Perfect for getting started.

### 2. ReceiverWithI2S
High-quality audio receiver using external I2S DAC (PCM5102, MAX98357A, etc.)

### 3. AdvancedReceiver
Demonstrates advanced features:
- Custom data processing callbacks
- Real-time audio statistics
- LED status indicators
- Data rate monitoring

Access examples in Arduino IDE: **File** → **Examples** → **BT Audio Receiver**

## 🔧 Troubleshooting

### Cannot Find Device
- Make sure Bluetooth is enabled on your phone/computer
- Check that `begin()` returned `true`
- Verify the ESP32 is powered properly
- Try rebooting the ESP32

### No Audio Output
- Check physical connections to your speaker/amplifier
- Verify correct GPIO pins are used
- Make sure `setupI2S()` or `useInternalDAC()` is called
- Check if audio is playing on the source device
- Verify the source device volume is not muted

### Poor Audio Quality
- Use external I2S DAC instead of internal DAC
- Add an RC filter to internal DAC output
- Check for loose connections
- Ensure stable power supply (minimum 500mA)
- Try different sample rates (44100 or 48000 Hz)

### Connection Drops
- Reduce distance between devices
- Remove obstacles between ESP32 and source device
- Check for WiFi interference (disable WiFi if not needed)
- Use a good quality power supply

### Compilation Errors
- Make sure you're using ESP32 board (not ESP8266)
- Update ESP32 Arduino Core to latest version
- Check that all required files are in the library folder

## 💡 Tips and Best Practices

1. **Power Supply**: Use a quality 5V power supply with at least 500mA capacity
2. **Audio Quality**: External I2S DACs provide much better quality than internal DAC
3. **Pin Selection**: You can use any available GPIO pins for I2S
4. **Amplification**: Connect the output to an amplifier for better volume
5. **Callbacks**: Use callbacks to add visual feedback (LEDs, displays)
6. **Serial Monitor**: Set baud rate to 115200 for debugging

## 🛠️ Compatible Hardware

### Tested ESP32 Boards
- ESP32 DevKit V1
- ESP32 WROOM-32
- ESP32 WROVER
- ESP-WROOM-32D
- NodeMCU-32S

### Recommended I2S DACs
- **PCM5102 / PCM5102A**: Excellent quality, easy to use, no configuration
- **MAX98357A**: I2S DAC + amplifier in one module, 3W output
- **UDA1334A**: Breakout board, good quality, easy wiring
- **CS4344**: Another good option for high-quality audio

## 📖 Technical Details

- **Protocol**: Bluetooth A2DP (Advanced Audio Distribution Profile) Sink
- **Bluetooth Version**: Bluetooth Classic (not BLE)
- **Audio Format**: 16-bit PCM
- **Sample Rates**: Typically 44.1kHz or 48kHz
- **Channels**: Stereo (2 channels)
- **Latency**: ~100-200ms (typical for Bluetooth audio)

## 🤝 Contributing

Contributions are welcome! Feel free to:
- Report bugs
- Suggest new features
- Submit pull requests
- Improve documentation

## 📄 License

This library is released under the MIT License. See LICENSE file for details.

## 🙏 Credits

Based on ESP32 A2DP library and examples from Espressif Systems.

## 📞 Support

For issues, questions, or suggestions:
- Open an issue on GitHub
- Check the examples folder for reference code
- Consult the ESP32 Arduino documentation

---

**Enjoy your ESP32 Bluetooth Speaker!** 🎶
