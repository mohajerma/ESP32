/*
 * RGB LED Control for ESP32 Dev Module
 * 
 * This sketch demonstrates basic ESP32 functionality with an RGB LED
 * 
 * Hardware Connections:
 * - Red LED Pin   -> GPIO 25
 * - Green LED Pin -> GPIO 26
 * - Blue LED Pin  -> GPIO 27
 * - Common Ground -> GND
 * 
 * Author: Your Name
 * Date: 2026-07-17
 */

#include "config.h"

// LED brightness values (0-255)
int redValue = 0;
int greenValue = 0;
int blueValue = 0;

// PWM properties
const int freq = 5000;        // PWM frequency in Hz
const int resolution = 8;     // 8-bit resolution (0-255)

void setup() {
  // Initialize serial communication
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("\n=================================");
  Serial.println("ESP32 RGB LED Control Starting...");
  Serial.println("=================================\n");
  
  // Configure LED PWM (ESP32 3.x API - automatically assigns channels)
  ledcAttach(RED_PIN, freq, resolution);
  ledcAttach(GREEN_PIN, freq, resolution);
  ledcAttach(BLUE_PIN, freq, resolution);
  
  Serial.println("Setup complete!");
  Serial.println("RGB LED initialized on pins:");
  Serial.printf("  Red:   GPIO %d\n", RED_PIN);
  Serial.printf("  Green: GPIO %d\n", GREEN_PIN);
  Serial.printf("  Blue:  GPIO %d\n", BLUE_PIN);
  Serial.println();
}

void loop() {
  // Cycle through colors
  Serial.println("Red");
  setColor(255, 0, 0);
  delay(1000);
  
  Serial.println("Green");
  setColor(0, 255, 0);
  delay(1000);
  
  Serial.println("Blue");
  setColor(0, 0, 255);
  delay(1000);
  
  Serial.println("Yellow");
  setColor(255, 255, 0);
  delay(1000);
  
  Serial.println("Cyan");
  setColor(0, 255, 255);
  delay(1000);
  
  Serial.println("Magenta");
  setColor(255, 0, 255);
  delay(1000);
  
  Serial.println("White");
  setColor(255, 255, 255);
  delay(1000);
  
  Serial.println("Off\n");
  setColor(0, 0, 0);
  delay(1000);
  
  // Rainbow fade effect
  Serial.println("Rainbow fade...");
  rainbowFade(10);
}

// Function to set RGB color
void setColor(int red, int green, int blue) {
  ledcWrite(RED_PIN, red);
  ledcWrite(GREEN_PIN, green);
  ledcWrite(BLUE_PIN, blue);
}

// Rainbow fade effect
void rainbowFade(int wait) {
  for (int i = 0; i < 256; i++) {
    int red = (i < 85) ? i * 3 : (i < 170) ? 255 - (i - 85) * 3 : 0;
    int green = (i < 85) ? 0 : (i < 170) ? (i - 85) * 3 : 255 - (i - 170) * 3;
    int blue = (i < 85) ? 255 - i * 3 : (i < 170) ? 0 : (i - 170) * 3;
    
    setColor(red, green, blue);
    delay(wait);
  }
}
