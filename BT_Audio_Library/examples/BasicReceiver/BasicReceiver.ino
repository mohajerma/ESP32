/*
 * Basic Bluetooth Audio Receiver Example
 * 
 * This example demonstrates the simplest way to use the BT_Audio_Receiver library
 * with the ESP32's internal DAC.
 * 
 * Hardware Connections (Internal DAC):
 * - GPIO25: Right channel audio output
 * - GPIO26: Left channel audio output
 * - Connect speaker/headphone through a simple RC filter or amplifier
 * 
 * Usage:
 * 1. Upload this sketch to your ESP32
 * 2. Power on the ESP32
 * 3. Search for "ESP32_BT_Speaker" on your phone/computer
 * 4. Connect and play audio!
 * 
 * Note: Internal DAC output is relatively low quality (8-bit).
 * For better audio quality, use external I2S DAC (see ReceiverWithI2S example).
 */

#include <BT_Audio_Receiver.h>

// Create Bluetooth audio receiver instance
BT_Audio_Receiver btAudio;

void setup() {
  // Initialize serial for debugging
  Serial.begin(115200);
  Serial.println("\n=== ESP32 Bluetooth Audio Receiver ===");
  Serial.println("Starting...");
  
  // Initialize the Bluetooth receiver with custom device name
  if (!btAudio.begin("ESP32_BT_Speaker")) {
    Serial.println("Failed to initialize Bluetooth!");
    while(1) delay(1000);
  }
  
  // Configure to use internal DAC (GPIO25 and GPIO26)
  if (!btAudio.useInternalDAC()) {
    Serial.println("Failed to initialize DAC!");
    while(1) delay(1000);
  }
  
  // Register connection state callback (optional)
  btAudio.onConnectionStateChanged([](esp_a2d_connection_state_t state, void* param) {
    if (state == ESP_A2D_CONNECTION_STATE_CONNECTED) {
      Serial.println("Device connected!");
    } else if (state == ESP_A2D_CONNECTION_STATE_DISCONNECTED) {
      Serial.println("Device disconnected!");
    }
  });
  
  // Register audio state callback (optional)
  btAudio.onAudioStateChanged([](esp_a2d_audio_state_t state, void* param) {
    if (state == ESP_A2D_AUDIO_STATE_STARTED) {
      Serial.println("Audio playback started");
    } else if (state == ESP_A2D_AUDIO_STATE_STOPPED) {
      Serial.println("Audio playback stopped");
    }
  });
  
  Serial.println("Bluetooth Audio Receiver Ready!");
  Serial.println("Device Name: " + String(btAudio.getDeviceName()));
  Serial.println("MAC Address: " + btAudio.getBluetoothAddress());
  Serial.println("Waiting for connection...");
}

void loop() {
  // Nothing to do in loop - audio streaming is handled automatically
  // You can add your custom code here
  
  // Example: Print connection status every 5 seconds
  static unsigned long lastPrint = 0;
  if (millis() - lastPrint > 5000) {
    lastPrint = millis();
    Serial.print("Status: ");
    Serial.println(btAudio.isConnected() ? "Connected" : "Waiting for connection");
  }
  
  delay(100);
}
