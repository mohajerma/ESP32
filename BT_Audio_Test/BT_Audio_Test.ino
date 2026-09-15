/*
 * Quick Test for BT_Audio_Receiver Library
 * 
 * This is a minimal test to verify the library works.
 * Monitor the Serial output to see status messages.
 * 
 * Hardware: Connect speaker to GPIO25/26 (or use external DAC)
 */

#include <BT_Audio_Receiver.h>

BT_Audio_Receiver btAudio;

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("\n========================================");
  Serial.println("  BT Audio Receiver Library Test");
  Serial.println("========================================\n");
  
  // Initialize Bluetooth
  Serial.print("Initializing Bluetooth... ");
  if (!btAudio.begin("ESP32_TEST_SPEAKER")) {
    Serial.println("FAILED!");
    Serial.println("Check if ESP32 Bluetooth is working.");
    while(1) {
      digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
      delay(200);  // Fast blink = error
    }
  }
  Serial.println("OK");
  
  // Setup internal DAC
  Serial.print("Setting up Internal DAC... ");
  if (!btAudio.useInternalDAC()) {
    Serial.println("FAILED!");
    while(1) delay(1000);
  }
  Serial.println("OK");
  
  // Setup callbacks for monitoring
  btAudio.onConnectionStateChanged([](esp_a2d_connection_state_t state, void* param) {
    Serial.println("\n>>> CONNECTION STATE CHANGE <<<");
    if (state == ESP_A2D_CONNECTION_STATE_CONNECTED) {
      Serial.println("STATUS: ✓ CONNECTED");
      Serial.println("You can now play audio from your device!");
      digitalWrite(LED_BUILTIN, HIGH);
    } else if (state == ESP_A2D_CONNECTION_STATE_DISCONNECTED) {
      Serial.println("STATUS: ✗ DISCONNECTED");
      digitalWrite(LED_BUILTIN, LOW);
    }
  });
  
  btAudio.onAudioStateChanged([](esp_a2d_audio_state_t state, void* param) {
    if (state == ESP_A2D_AUDIO_STATE_STARTED) {
      Serial.println("\n♫ AUDIO PLAYBACK STARTED ♫");
    } else if (state == ESP_A2D_AUDIO_STATE_STOPPED) {
      Serial.println("\n■ AUDIO PLAYBACK STOPPED");
    }
  });
  
  // Setup LED indicator
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
  
  // Print device info
  Serial.println("\n========================================");
  Serial.println("DEVICE INFORMATION:");
  Serial.println("========================================");
  Serial.print("Device Name:  ");
  Serial.println(btAudio.getDeviceName());
  Serial.print("MAC Address:  ");
  Serial.println(btAudio.getBluetoothAddress());
  Serial.println("\n========================================");
  Serial.println("READY! Follow these steps:");
  Serial.println("========================================");
  Serial.println("1. Open Bluetooth settings on your phone");
  Serial.println("2. Look for 'ESP32_TEST_SPEAKER'");
  Serial.println("3. Tap to connect (no pairing code needed)");
  Serial.println("4. Play music from any app");
  Serial.println("5. Check GPIO25/26 for audio output");
  Serial.println("========================================\n");
  Serial.println("Waiting for connection...\n");
}

void loop() {
  static unsigned long lastCheck = 0;
  static bool wasConnected = false;
  
  // Check connection status every second
  if (millis() - lastCheck >= 1000) {
    lastCheck = millis();
    
    bool isConnected = btAudio.isConnected();
    
    // Print periodic status if connected
    if (isConnected && wasConnected) {
      static int counter = 0;
      if (++counter >= 10) {  // Every 10 seconds
        counter = 0;
        Serial.println("♫ Still streaming...");
      }
    }
    
    wasConnected = isConnected;
  }
  
  delay(100);
}
