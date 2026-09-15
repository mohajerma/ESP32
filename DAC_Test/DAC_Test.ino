/*
 * ESP32 DAC Audio Test
 * 
 * This sketch plays a continuous 1kHz test tone through the ESP32's DAC
 * No Bluetooth required - just for testing audio output hardware
 * 
 * Hardware:
 * - Connect your headphones/amplifier to GPIO25 (DAC1) and GPIO26 (DAC2)
 * - Connect ground (GND)
 * 
 * You should hear a steady tone if your audio output is working
 */

#include <driver/dac.h>

// Sine wave table (one complete cycle, 64 samples)
const uint8_t sine_wave[64] = {
  128, 140, 152, 164, 176, 187, 198, 208, 
  218, 226, 234, 240, 245, 250, 253, 254, 
  255, 254, 253, 250, 245, 240, 234, 226, 
  218, 208, 198, 187, 176, 164, 152, 140, 
  128, 115, 103,  91,  79,  68,  57,  47, 
   37,  29,  21,  15,  10,   5,   2,   1, 
    0,   1,   2,   5,  10,  15,  21,  29, 
   37,  47,  57,  68,  79,  91, 103, 115
};

volatile uint32_t sample_index = 0;

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println();
  Serial.println("=====================================");
  Serial.println("ESP32 DAC Audio Test");
  Serial.println("=====================================");
  Serial.println();
  Serial.println("Hardware Setup:");
  Serial.println("  GPIO25 (DAC1) --> Headphone/Amp Left Input");
  Serial.println("  GPIO26 (DAC2) --> Headphone/Amp Right Input");
  Serial.println("  GND           --> Headphone/Amp Ground");
  Serial.println();
  
  // Enable DAC on GPIO25 (Channel 1) and GPIO26 (Channel 2)
  dac_output_enable(DAC_CHANNEL_1);  // GPIO25
  dac_output_enable(DAC_CHANNEL_2);  // GPIO26
  
  Serial.println("✓ DAC enabled on GPIO25 and GPIO26");
  Serial.println("✓ Playing 1kHz test tone (continuous sine wave)");
  Serial.println();
  Serial.println("=====================================");
  Serial.println("Playing continuous 1kHz test tone...");
  Serial.println("You should hear a steady beep sound.");
  Serial.println("=====================================");
  Serial.println();
  Serial.println("Multimeter Test (while tone is playing):");
  Serial.println("  Measure GPIO25 to GND: Should show ~1.65V DC");
  Serial.println("  Measure GPIO26 to GND: Should show ~1.65V DC");
  Serial.println("  (voltage will fluctuate slightly with the tone)");
  Serial.println();
  Serial.println("If you hear nothing:");
  Serial.println("  1. Check your wiring");
  Serial.println("  2. You need an amplifier - DAC is too weak for headphones");
  Serial.println("  3. Test voltage with multimeter to verify DAC is working");
}

void loop() {
  // Play the sine wave continuously
  // This creates approximately a 1kHz tone
  for (int i = 0; i < 64; i++) {
    dac_output_voltage(DAC_CHANNEL_1, sine_wave[i]);  // GPIO25 (Left)
    dac_output_voltage(DAC_CHANNEL_2, sine_wave[i]);  // GPIO26 (Right)
    delayMicroseconds(22);  // ~1000Hz (64 samples * 1000Hz = 64000 samples/sec, 1000000/64000 ≈ 15.6µs, adjusted for loop overhead)
  }
  
  // Show status every ~1000 cycles (about 1 second)
  static unsigned long cycle_count = 0;
  static unsigned long start_time = millis();
  cycle_count++;
  
  if (cycle_count % 1000 == 0) {
    unsigned long now = millis();
    Serial.print("[Running] Tone playing for ");
    Serial.print((now - start_time) / 1000);
    Serial.print(" seconds | Free Heap: ");
    Serial.print(ESP.getFreeHeap());
    Serial.println(" bytes");
  }
}