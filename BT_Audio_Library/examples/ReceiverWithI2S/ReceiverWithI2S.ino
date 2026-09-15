/*
 * Bluetooth Audio Receiver with External I2S DAC Example
 * 
 * This example shows how to use the BT_Audio_Receiver library
 * with an external I2S DAC for high-quality audio output.
 * 
 * Supported DACs:
 * - PCM5102 / PCM5102A (Common, no configuration needed)
 * - MAX98357A (I2S amplifier with built-in DAC)
 * - UDA1334A
 * - And any other I2S compatible DAC
 * 
 * Default Hardware Connections:
 * ESP32    -->   I2S DAC
 * GPIO26   -->   BCK  (Bit Clock)
 * GPIO25   -->   WS   (Word Select / LRC)
 * GPIO22   -->   DATA (Data In)
 * GND      -->   GND
 * 3.3V/5V  -->   VIN
 * 
 * For PCM5102:
 * - Connect SCK to GND (for auto-detect mode)
 * - Connect FLT, DEMP, FMT to GND
 * - Connect XMT to 3.3V or float
 * 
 * You can customize the pin configuration in the setupI2S() call.
 */

#include <BT_Audio_Receiver.h>

// Create Bluetooth audio receiver instance
BT_Audio_Receiver btAudio;

// I2S Pin Configuration (customize if needed)
const int I2S_BCK_PIN  = 26;  // Bit Clock
const int I2S_WS_PIN   = 25;  // Word Select (LRC)
const int I2S_DATA_PIN = 22;  // Data Out

void setup() {
  // Initialize serial for debugging
  Serial.begin(115200);
  Serial.println("\n=== ESP32 Bluetooth Audio Receiver with I2S DAC ===");
  Serial.println("Starting...");
  
  // Initialize the Bluetooth receiver with custom device name
  if (!btAudio.begin("ESP32_HiFi_Speaker")) {
    Serial.println("Failed to initialize Bluetooth!");
    while(1) delay(1000);
  }
  
  // Configure I2S for external DAC
  // Parameters: BCK pin, WS pin, DATA pin, Sample Rate
  if (!btAudio.setupI2S(I2S_BCK_PIN, I2S_WS_PIN, I2S_DATA_PIN, 44100)) {
    Serial.println("Failed to initialize I2S!");
    while(1) delay(1000);
  }
  
  // Set volume (0-127)
  btAudio.setVolume(100);
  
  // Register connection state callback (optional)
  btAudio.onConnectionStateChanged([](esp_a2d_connection_state_t state, void* param) {
    if (state == ESP_A2D_CONNECTION_STATE_CONNECTED) {
      Serial.println("✓ Device connected!");
    } else if (state == ESP_A2D_CONNECTION_STATE_DISCONNECTED) {
      Serial.println("✗ Device disconnected!");
    }
  });
  
  // Register audio state callback (optional)
  btAudio.onAudioStateChanged([](esp_a2d_audio_state_t state, void* param) {
    if (state == ESP_A2D_AUDIO_STATE_STARTED) {
      Serial.println("♫ Audio playback started");
    } else if (state == ESP_A2D_AUDIO_STATE_STOPPED) {
      Serial.println("■ Audio playback stopped");
    } else if (state == ESP_A2D_AUDIO_STATE_REMOTE_SUSPEND) {
      Serial.println("⏸ Audio suspended");
    }
  });
  
  Serial.println("\n✓ Bluetooth Audio Receiver Ready!");
  Serial.println("Device Name: " + String(btAudio.getDeviceName()));
  Serial.println("MAC Address: " + btAudio.getBluetoothAddress());
  Serial.println("I2S Configuration:");
  Serial.println("  BCK:  GPIO" + String(I2S_BCK_PIN));
  Serial.println("  WS:   GPIO" + String(I2S_WS_PIN));
  Serial.println("  DATA: GPIO" + String(I2S_DATA_PIN));
  Serial.println("  Sample Rate: 44100 Hz");
  Serial.println("\nWaiting for connection...");
}

void loop() {
  // Nothing to do in loop - audio streaming is handled automatically
  // You can add your custom code here
  
  // Example: Show connection status with more detail
  static unsigned long lastPrint = 0;
  static bool wasConnected = false;
  
  bool isConnected = btAudio.isConnected();
  
  // Print status change
  if (isConnected != wasConnected) {
    wasConnected = isConnected;
    if (isConnected) {
      Serial.println("\n>>> Device Connected <<<");
      Serial.println("You can now play audio from your device");
    } else {
      Serial.println("\n>>> Device Disconnected <<<");
      Serial.println("Waiting for new connection...");
    }
  }
  
  // Periodic status update
  if (millis() - lastPrint > 10000) {
    lastPrint = millis();
    if (isConnected) {
      Serial.print("♫ Streaming... [Volume: ");
      Serial.print(btAudio.getVolume());
      Serial.println("]");
    }
  }
  
  delay(100);
}
