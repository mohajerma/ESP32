/*
 * Advanced Bluetooth Audio Receiver Example
 * 
 * This example demonstrates advanced features of the BT_Audio_Receiver library:
 * - Custom data processing callback
 * - Audio data monitoring
 * - Real-time statistics
 * - LED indicators for connection/playback status
 * 
 * Hardware:
 * - External I2S DAC (PCM5102 or similar)
 * - LED on GPIO2 for connection status
 * - LED on GPIO4 for audio playback status (optional)
 */

#include <BT_Audio_Receiver.h>

// Create Bluetooth audio receiver instance
BT_Audio_Receiver btAudio;

// I2S Pin Configuration
const int I2S_BCK_PIN  = 26;
const int I2S_WS_PIN   = 25;
const int I2S_DATA_PIN = 22;

// LED pins for status indication
const int LED_CONNECTION_PIN = 2;  // Built-in LED
const int LED_PLAYBACK_PIN = 4;    // External LED (optional)

// Statistics
unsigned long totalBytesReceived = 0;
unsigned long lastStatsTime = 0;
float dataRateKBps = 0;

void setup() {
  Serial.begin(115200);
  Serial.println("\n=== ESP32 Bluetooth Audio Receiver - Advanced Example ===");
  
  // Setup LED pins
  pinMode(LED_CONNECTION_PIN, OUTPUT);
  pinMode(LED_PLAYBACK_PIN, OUTPUT);
  digitalWrite(LED_CONNECTION_PIN, LOW);
  digitalWrite(LED_PLAYBACK_PIN, LOW);
  
  // Initialize Bluetooth receiver
  if (!btAudio.begin("ESP32_Advanced_BT")) {
    Serial.println("Failed to initialize Bluetooth!");
    while(1) delay(1000);
  }
  
  // Setup I2S DAC
  if (!btAudio.setupI2S(I2S_BCK_PIN, I2S_WS_PIN, I2S_DATA_PIN, 44100)) {
    Serial.println("Failed to initialize I2S!");
    while(1) delay(1000);
  }
  
  // Register data received callback for monitoring
  btAudio.onDataReceived([](const uint8_t *data, uint32_t len) {
    totalBytesReceived += len;
    // You could process the audio data here if needed
    // For example: analyze volume, apply effects, etc.
  });
  
  // Register connection state callback
  btAudio.onConnectionStateChanged([](esp_a2d_connection_state_t state, void* param) {
    if (state == ESP_A2D_CONNECTION_STATE_CONNECTED) {
      Serial.println("\n✓ CONNECTED");
      digitalWrite(LED_CONNECTION_PIN, HIGH);
      totalBytesReceived = 0;
      lastStatsTime = millis();
    } else if (state == ESP_A2D_CONNECTION_STATE_DISCONNECTED) {
      Serial.println("\n✗ DISCONNECTED");
      digitalWrite(LED_CONNECTION_PIN, LOW);
      digitalWrite(LED_PLAYBACK_PIN, LOW);
    }
  });
  
  // Register audio state callback
  btAudio.onAudioStateChanged([](esp_a2d_audio_state_t state, void* param) {
    if (state == ESP_A2D_AUDIO_STATE_STARTED) {
      Serial.println("♫ PLAYING");
      digitalWrite(LED_PLAYBACK_PIN, HIGH);
    } else if (state == ESP_A2D_AUDIO_STATE_STOPPED) {
      Serial.println("■ STOPPED");
      digitalWrite(LED_PLAYBACK_PIN, LOW);
    }
  });
  
  Serial.println("\n✓ System Ready!");
  Serial.println("Device: " + String(btAudio.getDeviceName()));
  Serial.println("Address: " + btAudio.getBluetoothAddress());
  Serial.println("\nWaiting for connection...\n");
}

void loop() {
  // Calculate and display statistics every 2 seconds
  if (millis() - lastStatsTime >= 2000 && btAudio.isConnected()) {
    unsigned long elapsed = millis() - lastStatsTime;
    dataRateKBps = (totalBytesReceived / 1024.0) / (elapsed / 1000.0);
    
    Serial.print("Data Rate: ");
    Serial.print(dataRateKBps, 2);
    Serial.print(" KB/s | Total: ");
    Serial.print(totalBytesReceived / 1024);
    Serial.println(" KB");
    
    totalBytesReceived = 0;
    lastStatsTime = millis();
  }
  
  delay(100);
}
