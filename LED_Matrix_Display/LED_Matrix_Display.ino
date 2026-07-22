/*
 * LED Matrix Display Controller
 * ESP32 project for controlling addressable RGB LED strips
 * Features: Moving text, graphics, WiFi and Bluetooth control
 * LED Type: WS2812B
 * Library: FastLED
 */

#include <FastLED.h>
#include <WiFi.h>
#include <WebServer.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include "config.h"

// Forward declarations
void setMode(String mode);
void setText(String text);
void setBrightness(int value);
void setScrollSpeed(int speed);
void updateDisplay();

#include "led_effects.h"
#include "wifi_server.h"
#include "ble_server.h"

// LED array
CRGB leds[NUM_LEDS];

// Current display mode
String currentMode = "rainbow";
String currentText = "Hello ESP32!";
int scrollSpeed = 50;
int brightness = 128;

// Server instances
WebServer webServer(80);
BLEServer* pBLEServer = NULL;
BLECharacteristic* pCharacteristic = NULL;
BLECharacteristic* pTxCharacteristic = NULL;
bool deviceConnected = false;

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("\n=================================");
  Serial.println("LED Matrix Display Controller");
  Serial.println("=================================\n");
  
  // Initialize LED strip
  initLEDs();
  
  // Initialize WiFi
  if (ENABLE_WIFI) {
    initWiFi();
    setupWebServer();
  }
  
  // Initialize Bluetooth
  if (ENABLE_BLUETOOTH) {
    initBLE();
  }
  
  Serial.println("\nSetup complete!");
  Serial.println("Ready to receive commands...\n");
}

void loop() {
  // Handle WiFi client requests
  if (ENABLE_WIFI) {
    webServer.handleClient();
  }
  
  // Handle Bluetooth updates
  if (ENABLE_BLUETOOTH) {
    handleBLE();
  }
  
  // Update LED display based on current mode
  updateDisplay();
  
  delay(10);
}

// Initialize LED strip
void initLEDs() {
  Serial.println("Initializing LED strip...");
  
  #ifdef LED_PIN
    FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS);
  #else
    FastLED.addLeds<LED_TYPE, LED_DATA_PIN, LED_CLOCK_PIN, COLOR_ORDER>(leds, NUM_LEDS);
  #endif
  
  FastLED.setBrightness(brightness);
  FastLED.clear();
  FastLED.show();
  
  // Startup animation
  for(int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CRGB::Blue;
    FastLED.show();
    delay(20);
  }
  FastLED.clear();
  FastLED.show();
  
  Serial.println("LED strip initialized!");
  Serial.printf("Number of LEDs: %d\n", NUM_LEDS);
}

// Update the LED display based on current mode
void updateDisplay() {
  static unsigned long lastUpdate = 0;
  unsigned long currentTime = millis();
  
  if (currentTime - lastUpdate < scrollSpeed) {
    return;
  }
  lastUpdate = currentTime;
  
  if (currentMode == "rainbow") {
    effectRainbow();
  } 
  else if (currentMode == "scroll_text") {
    effectScrollText(currentText);
  }
  else if (currentMode == "color_wipe") {
    effectColorWipe(CRGB::Red);
  }
  else if (currentMode == "theater_chase") {
    effectTheaterChase(CRGB::Blue);
  }
  else if (currentMode == "fire") {
    effectFire();
  }
  else if (currentMode == "confetti") {
    effectConfetti();
  }
  else if (currentMode == "solid") {
    fill_solid(leds, NUM_LEDS, CRGB::White);
  }
  else if (currentMode == "off") {
    FastLED.clear();
  }
  
  FastLED.show();
}

// Set display mode
void setMode(String mode) {
  currentMode = mode;
  Serial.println("Mode changed to: " + mode);
}

// Set scrolling text
void setText(String text) {
  currentText = text;
  Serial.println("Text set to: " + text);
}

// Set brightness (0-255)
void setBrightness(int value) {
  brightness = constrain(value, 0, 255);
  FastLED.setBrightness(brightness);
  Serial.printf("Brightness set to: %d\n", brightness);
}

// Set scroll speed (milliseconds)
void setScrollSpeed(int speed) {
  scrollSpeed = constrain(speed, 10, 500);
  Serial.printf("Scroll speed set to: %d ms\n", scrollSpeed);
}
