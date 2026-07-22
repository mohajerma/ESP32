/*
 * Configuration file for LED Matrix Display
 * Modify these settings according to your setup
 */

#ifndef CONFIG_H
#define CONFIG_H

// ========================
// LED Strip Configuration
// ========================

// Number of LEDs in your strip
#define NUM_LEDS 60

// LED strip type (WS2812B, WS2811, SK6812, etc.)
#define LED_TYPE WS2812B

// Color order (RGB, GRB, BRG, etc.)
#define COLOR_ORDER GRB

// For 3-wire LEDs (WS2812B, WS2811, etc.)
#define LED_PIN 18  // GPIO pin connected to LED strip data line

// For 4-wire LEDs (APA102, SK9822, etc.) - uncomment if needed
// #define LED_DATA_PIN 5
// #define LED_CLOCK_PIN 18

// Maximum brightness (0-255)
#define MAX_BRIGHTNESS 255

// Default brightness (0-255)
#define DEFAULT_BRIGHTNESS 128

// ========================
// WiFi Configuration
// ========================

#define ENABLE_WIFI true

// WiFi credentials
#define WIFI_SSID "Mohajer"
#define WIFI_PASSWORD "gdlamborgini"

// Access Point mode (if you want ESP32 to create its own WiFi network)
#define AP_MODE true
#define AP_SSID "LED_Matrix_AP"
#define AP_PASSWORD "12345678"

// Web server port
#define WEB_SERVER_PORT 80

// ========================
// Bluetooth Configuration
// ========================

#define ENABLE_BLUETOOTH true

// Bluetooth device name
#define BLE_DEVICE_NAME "LED_Matrix_Display"

// BLE Service and Characteristic UUIDs (Nordic UART Service standard)
#define SERVICE_UUID        "6E400001-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_UUID_RX "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"  // Write
#define CHARACTERISTIC_UUID_TX "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"  // Notify

// ========================
// Display Configuration
// ========================

// Default effect mode
#define DEFAULT_MODE "rainbow"

// Default scroll speed (milliseconds between updates)
#define DEFAULT_SCROLL_SPEED 50

// For matrix displays (if arranging LEDs in a grid)
#define MATRIX_WIDTH 10   // Number of LEDs wide
#define MATRIX_HEIGHT 6   // Number of LEDs tall
#define MATRIX_TYPE VERTICAL_ZIGZAG  // or HORIZONTAL_STRAIGHT, etc.

// ========================
// Performance Settings
// ========================

// Frames per second
#define FRAMES_PER_SECOND 60

// WiFi connection timeout (milliseconds)
#define WIFI_TIMEOUT 10000

#endif // CONFIG_H
