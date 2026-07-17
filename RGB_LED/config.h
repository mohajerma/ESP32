/*
 * Configuration file for ESP32 RGB LED project
 * 
 * Define your pin connections and constants here
 */

#ifndef CONFIG_H
#define CONFIG_H

// Pin definitions for RGB LED
#define RED_PIN     25    // GPIO pin for Red LED
#define GREEN_PIN   26    // GPIO pin for Green LED
#define BLUE_PIN    27    // GPIO pin for Blue LED

// WiFi credentials (uncomment and fill in if needed)
// #define WIFI_SSID     "YourSSID"
// #define WIFI_PASSWORD "YourPassword"

// Other configuration constants
#define SERIAL_BAUD_RATE  115200
#define LED_BRIGHTNESS    255     // Max brightness (0-255)

#endif // CONFIG_H
