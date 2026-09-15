/*
 * Minimal Bluetooth A2DP Test 
 * Tests if basic Bluetooth A2DP works on this ESP32
 */

#include "BluetoothA2DPSink.h"

BluetoothA2DPSink a2dp_sink;

void setup() {
  Serial.begin(115200);
  Serial.println("\n=== Minimal BT A2DP Test ===");
  
  a2dp_sink.start("ESP32_Test_Speaker");
  
  Serial.println("Bluetooth A2DP sink started");
  Serial.println("Connect from your phone and play audio");
}

void loop() {
  delay(1000);
}
