# Testing Guide for BT Audio Receiver Library

## Current Status
✅ Library compiles successfully  
✅ Code uploads to ESP32  
⚠️ Bluetooth initialization needs fixing  

## Error Analysis
**Error Code**: `0x103` = `ESP_ERR_INVALID_STATE`  
**Meaning**: Bluetooth controller initialization sequence issue

## Possible Causes
1. BLE memory release timing
2. ESP32 Arduino Core 3.3.10 compatibility
3. Board configuration/partition scheme
4. NVS initialization order

## Solutions to Try

### Solution 1: Use Different Partition Scheme
The default partition might not have enough space for Bluetooth Classic:

```powershell
# Try with minimal partitions
C:\code\ESP32\Tools\arduino-cli.exe compile --fqbn esp32:esp32:esp32:PartitionScheme=min_spiffs .

# Or try with no OTA
C:\code\ESP32\Tools\arduino-cli.exe compile --fqbn esp32:esp32:esp32:PartitionScheme=no_ota .
```

### Solution 2: Erase Flash Completely
Sometimes old Bluetooth data causes conflicts:

```powershell
C:\code\ESP32\Tools\esptool.exe --chip esp32 --port COM3 erase_flash
```

Then re-upload the sketch.

### Solution 3: Use Proven ESP32-A2DP Library
Test if your hardware works at all:

```powershell
# Install library
C:\code\ESP32\Tools\arduino-cli.exe lib install "ESP32-A2DP"

# Create test sketch (see BT_Simple_Test folder)
# Upload and test
```

### Solution 4: Modify Library Init (Advanced)
The library needs adjustment for ESP32 Arduino Core 3.3.10. The initialization sequence needs to match the core's expectations.

## Hardware Test Checklist

### Internal DAC Test (Simplest)
- [ ] ESP32 powered via USB
- [ ] GPIO25 → Headphone Right Channel
- [ ] GPIO26 → Headphone Left Channel  
- [ ] Common Ground
- [ ] Upload code
- [ ] Search for "ESP32_BT_Speaker" on phone
- [ ] Connect and play audio
- [ ] Check GPIO25/26 with multimeter (should see voltage changes)

### External I2S DAC Test (Better Quality)
- [ ] PCM5102 or MAX98357A connected
- [ ] GPIO26 → BCK
- [ ] GPIO25 → WS (LRC)
- [ ] GPIO22 → DATA
- [ ] Power and Ground connected
- [ ] Upload ReceiverWithI2S example
- [ ] Test connection

## Troubleshooting Commands

### Check ESP32 Info
```powershell
C:\code\ESP32\Tools\esptool.exe --chip esp32 --port COM3 chip_id
C:\code\ESP32\Tools\esptool.exe --chip esp32 --port COM3 flash_id
```

### Monitor with Detailed Logs
```powershell
C:\code\ESP32\Tools\arduino-cli.exe monitor -p COM3 -c baudrate=115200
```

### Try Different Board Settings
```powershell
# List all board options
C:\code\ESP32\Tools\arduino-cli.exe board listall esp32

# Try different flash modes
--fqbn esp32:esp32:esp32:FlashMode=qio
--fqbn esp32:esp32:esp32:FlashMode=dio
--fqbn esp32:esp32:esp32:FlashFreq=40
```

## Next Steps

1. **Immediate**: Try Solution 3 (ESP32-A2DP library) to verify hardware works
2. **Short-term**: Erase flash (Solution 2) and retry
3. **Long-term**: Update library initialization to match ESP32 Core 3.3.10

## Working Alternative

While we debug the initialization, you can use the proven [ESP32-A2DP library](https://github.com/pschatzmann/ESP32-A2DP):

```cpp
#include "BluetoothA2DPSink.h"

BluetoothA2DPSink a2dp_sink;

void setup() {
  // Simple one-liner!
  a2dp_sink.start("My_ESP32_Speaker");
}

void loop() {
  // Audio streams automatically
}
```

This library has been tested extensively and handles all the initialization complexities.

## Support

If issues persist:
1. Check ESP32 board version (should support Bluetooth)
2. Verify power supply (>= 500mA)
3. Try a different USB cable/port
4. Test with proven ESP32-A2DP library first
5. Share full serial output for analysis
