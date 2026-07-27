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
#include <Preferences.h>
#include "config.h"

// LED array
CRGB leds[NUM_LEDS];

// Active LED configuration (can differ from NUM_LEDS if set via web interface)
int activeLEDCount = NUM_LEDS;
int activeMatrixWidth = MATRIX_WIDTH;
int activeMatrixHeight = MATRIX_HEIGHT;

// Forward declarations
void setMode(String mode);
void setText(String text);
void setBrightness(int value);
void setScrollSpeed(int speed);
void updateDisplay();

#include "led_effects.h"
#include "wifi_server.h"
#include "ble_server.h"

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
  
  // Load saved configuration from NVS
  loadConfiguration();
  
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

// Load configuration from NVS (persistent storage)
void loadConfiguration() {
  Preferences prefs;
  prefs.begin("led-config", true); // Read-only
  
  // Load saved matrix dimensions
  int savedWidth = prefs.getInt("matrix_w", 0);
  int savedHeight = prefs.getInt("matrix_h", 0);
  int savedNumLEDs = prefs.getInt("num_leds", 0);
  
  prefs.end();
  
  // Use saved values if available, otherwise use config.h defaults
  if (savedWidth > 0 && savedHeight > 0 && savedNumLEDs > 0) {
    activeMatrixWidth = savedWidth;
    activeMatrixHeight = savedHeight;
    activeLEDCount = savedNumLEDs;
    
    // Validate against array size
    if (activeLEDCount > MAX_LEDS) {
      Serial.println("⚠️  WARNING: Saved LED count exceeds array capacity!");
      Serial.printf("   Saved: %d LEDs, Array capacity: %d LEDs\n", activeLEDCount, MAX_LEDS);
      Serial.println("   Using array capacity instead. Update MAX_LEDS in config.h for more LEDs.");
      activeLEDCount = MAX_LEDS;
    }
    
    Serial.println("✓ Loaded saved configuration from NVS:");
  } else {
    // No saved config, use defaults from config.h
    activeLEDCount = NUM_LEDS;
    activeMatrixWidth = MATRIX_WIDTH;
    activeMatrixHeight = MATRIX_HEIGHT;
    Serial.println("✓ Using default configuration from config.h:");
  }
  
  Serial.printf("  Matrix: %dx%d\n", activeMatrixWidth, activeMatrixHeight);
  Serial.printf("  Total LEDs: %d\n", activeLEDCount);
  Serial.printf("  Array capacity: %d\n\n", MAX_LEDS);
}

// Initialize LED strip
void initLEDs() {
  Serial.println("Initializing LED strip...");
  
  #ifdef LED_PIN
    FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, activeLEDCount);
  #else
    FastLED.addLeds<LED_TYPE, LED_DATA_PIN, LED_CLOCK_PIN, COLOR_ORDER>(leds, activeLEDCount);
  #endif
  
  FastLED.setBrightness(brightness);
  FastLED.clear();
  FastLED.show();
  
  // Startup animation
  for(int i = 0; i < activeLEDCount; i++) {
    leds[i] = CRGB::Blue;
    FastLED.show();
    delay(20);
  }
  FastLED.clear();
  FastLED.show();
  
  Serial.println("LED strip initialized!");
  Serial.printf("Active LEDs: %d\n", activeLEDCount);
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
