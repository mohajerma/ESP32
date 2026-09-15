/*
 * BT_Audio_Receiver Library for ESP32
 * 
 * A simple and easy-to-use library for receiving Bluetooth audio streams
 * on ESP32 using the A2DP (Advanced Audio Distribution Profile) protocol.
 * 
 * Features:
 * - Acts as a Bluetooth A2DP sink (receiver)
 * - Supports I2S output for external DACs
 * - Built-in DAC support (GPIO25/GPIO26)
 * - Callback support for audio data processing
 * - Device name customization
 * - Volume control
 * 
 * Author: ESP32 Dev
 * Version: 1.0.0
 * License: MIT
 */

#ifndef BT_AUDIO_RECEIVER_H
#define BT_AUDIO_RECEIVER_H

#include <Arduino.h>
#include "esp_a2dp_api.h"
#include "esp_bt.h"
#include "esp_bt_main.h"
#include "esp_bt_device.h"
#include "esp_gap_bt_api.h"
#include "driver/i2s.h"

// Default configuration values
#define BT_AUDIO_DEFAULT_DEVICE_NAME "ESP32_BT_Speaker"
#define BT_AUDIO_DEFAULT_SAMPLE_RATE 44100
#define BT_AUDIO_DEFAULT_BITS_PER_SAMPLE 16

// I2S pin definitions (can be customized)
#define I2S_DEFAULT_BCK_IO    26
#define I2S_DEFAULT_WS_IO     25
#define I2S_DEFAULT_DATA_OUT  22

// Callback function types
typedef void (*bt_audio_data_callback_t)(const uint8_t *data, uint32_t len);
typedef void (*bt_connection_state_callback_t)(esp_a2d_connection_state_t state, void* param);
typedef void (*bt_audio_state_callback_t)(esp_a2d_audio_state_t state, void* param);

class BT_Audio_Receiver {
public:
    // Constructor
    BT_Audio_Receiver();
    
    // Destructor
    ~BT_Audio_Receiver();
    
    // Basic setup methods
    bool begin(const char* deviceName = BT_AUDIO_DEFAULT_DEVICE_NAME);
    void end();
    
    // I2S configuration for external DAC
    bool setupI2S(int bckPin = I2S_DEFAULT_BCK_IO, 
                  int wsPin = I2S_DEFAULT_WS_IO, 
                  int dataPin = I2S_DEFAULT_DATA_OUT,
                  int sampleRate = BT_AUDIO_DEFAULT_SAMPLE_RATE);
    
    // Use internal DAC (GPIO25 and GPIO26)
    bool useInternalDAC();
    
    // Set device name (must be called before begin())
    void setDeviceName(const char* name);
    
    // Get current device name
    const char* getDeviceName();
    
    // Connection status
    bool isConnected();
    
    // Volume control (0-127)
    void setVolume(uint8_t volume);
    uint8_t getVolume();
    
    // Callback registration
    void onDataReceived(bt_audio_data_callback_t callback);
    void onConnectionStateChanged(bt_connection_state_callback_t callback);
    void onAudioStateChanged(bt_audio_state_callback_t callback);
    
    // Get Bluetooth MAC address
    String getBluetoothAddress();
    
    // Reconnect/disconnect
    void disconnect();
    bool reconnect();
    
private:
    // Internal state
    char _deviceName[32];
    bool _initialized;
    bool _connected;
    uint8_t _volume;
    bool _useI2S;
    bool _useInternalDAC;
    
    // I2S configuration
    int _bckPin;
    int _wsPin;
    int _dataPin;
    int _sampleRate;
    
    // Callbacks
    bt_audio_data_callback_t _dataCallback;
    bt_connection_state_callback_t _connectionCallback;
    bt_audio_state_callback_t _audioStateCallback;
    
    // Static instance pointer for callbacks
    static BT_Audio_Receiver* _instance;
    
    // Internal initialization methods
    bool initBluetooth();
    bool initA2DP();
    bool initI2S();
    
    // Static callback wrappers for ESP32 API
    static void a2dpCallback(esp_a2d_cb_event_t event, esp_a2d_cb_param_t *param);
    static void dataCallback(const uint8_t *data, uint32_t len);
    
    // Helper methods
    void logStatus(const char* message);
};

#endif // BT_AUDIO_RECEIVER_H
