/*
 * BT_Audio_Receiver Library for ESP32 - Implementation
 */

#include "BT_Audio_Receiver.h"
#include "nvs_flash.h"

// Static instance pointer
BT_Audio_Receiver* BT_Audio_Receiver::_instance = nullptr;

// Constructor
BT_Audio_Receiver::BT_Audio_Receiver() {
    _initialized = false;
    _connected = false;
    _volume = 100;
    _useI2S = false;
    _useInternalDAC = false;
    _dataCallback = nullptr;
    _connectionCallback = nullptr;
    _audioStateCallback = nullptr;
    _bckPin = I2S_DEFAULT_BCK_IO;
    _wsPin = I2S_DEFAULT_WS_IO;
    _dataPin = I2S_DEFAULT_DATA_OUT;
    _sampleRate = BT_AUDIO_DEFAULT_SAMPLE_RATE;
    strcpy(_deviceName, BT_AUDIO_DEFAULT_DEVICE_NAME);
    _instance = this;
}

// Destructor
BT_Audio_Receiver::~BT_Audio_Receiver() {
    end();
    _instance = nullptr;
}

// Set device name (must be called before begin())
void BT_Audio_Receiver::setDeviceName(const char* name) {
    if (name && strlen(name) < 32) {
        strcpy(_deviceName, name);
    }
}

// Get current device name
const char* BT_Audio_Receiver::getDeviceName() {
    return _deviceName;
}

// Initialize Bluetooth
bool BT_Audio_Receiver::initBluetooth() {
    esp_err_t ret;
    
    logStatus("Starting Bluetooth initialization...");
    
    // Step 1: Initialize NVS first
    ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        logStatus("Erasing NVS...");
        nvs_flash_erase();
        ret = nvs_flash_init();
    }
    if (ret != ESP_OK) {
        char msg[64];
        sprintf(msg, "NVS init failed: 0x%x", ret);
        logStatus(msg);
        return false;
    }
    logStatus("NVS initialized");
    
    // Step 2: Check and deinitialize if already initialized
    esp_bt_controller_status_t status = esp_bt_controller_get_status();
    logStatus("Checking BT controller status...");
    
    if (status == ESP_BT_CONTROLLER_STATUS_ENABLED) {
        logStatus("BT controller already enabled, disabling...");
        esp_bluedroid_disable();
        esp_bluedroid_deinit();
        esp_bt_controller_disable();
        esp_bt_controller_deinit();
    } else if (status == ESP_BT_CONTROLLER_STATUS_INITED) {
        logStatus("BT controller already initialized, deinitializing...");
        esp_bt_controller_deinit();
    }
    
    // Step 3: Release BLE memory
    ret = esp_bt_controller_mem_release(ESP_BT_MODE_BLE);
    if (ret == ESP_OK) {
        logStatus("BLE memory released");
    } else if (ret == ESP_ERR_INVALID_STATE) {
        logStatus("BLE memory already released");
    } else {
        char msg[64];
        sprintf(msg, "BLE memory release warning: 0x%x", ret);
        logStatus(msg);
    }
    
    // Step 4: Initialize Bluetooth controller
    logStatus("Initializing BT controller...");
    esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    
    ret = esp_bt_controller_init(&bt_cfg);
    if (ret != ESP_OK) {
        char msg[64];
        sprintf(msg, "BT controller init failed: 0x%x", ret);
        logStatus(msg);
        return false;
    }
    logStatus("BT controller initialized");
    
    // Step 5: Enable Bluetooth controller
    logStatus("Enabling BT controller...");
    ret = esp_bt_controller_enable(ESP_BT_MODE_CLASSIC_BT);
    if (ret != ESP_OK) {
        char msg[64];
        sprintf(msg, "BT controller enable failed: 0x%x", ret);
        logStatus(msg);
        return false;
    }
    logStatus("BT controller enabled");
    
    // Step 6: Initialize Bluedroid
    logStatus("Initializing Bluedroid...");
    ret = esp_bluedroid_init();
    if (ret != ESP_OK) {
        char msg[64];
        sprintf(msg, "Bluedroid init failed: 0x%x", ret);
        logStatus(msg);
        return false;
    }
    logStatus("Bluedroid initialized");
    
    // Step 7: Enable Bluedroid
    logStatus("Enabling Bluedroid...");
    ret = esp_bluedroid_enable();
    if (ret != ESP_OK) {
        char msg[64];
        sprintf(msg, "Bluedroid enable failed: 0x%x", ret);
        logStatus(msg);
        return false;
    }
    logStatus("Bluedroid enabled - Bluetooth ready!");
    
    return true;
}

// Initialize A2DP
bool BT_Audio_Receiver::initA2DP() {
    // Register A2DP callback
    esp_a2d_register_callback(a2dpCallback);
    
    // Register data callback
    esp_a2d_sink_register_data_callback(dataCallback);
    
    // Initialize A2DP sink
    if (esp_a2d_sink_init() != ESP_OK) {
        logStatus("A2DP sink init failed");
        return false;
    }
    
    // Set discoverable and connectable mode
    esp_bt_gap_set_scan_mode(ESP_BT_CONNECTABLE, ESP_BT_GENERAL_DISCOVERABLE);
    
    // Set device name
    esp_bt_dev_set_device_name(_deviceName);
    
    return true;
}

// Initialize I2S
bool BT_Audio_Receiver::initI2S() {
    i2s_config_t i2s_config;
    
    if (_useInternalDAC) {
        // Configuration for internal DAC
        i2s_config = {
            .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX | I2S_MODE_DAC_BUILT_IN),
            .sample_rate = _sampleRate,
            .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
            .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
            .communication_format = I2S_COMM_FORMAT_STAND_MSB,
            .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
            .dma_buf_count = 8,
            .dma_buf_len = 64,
            .use_apll = false,
            .tx_desc_auto_clear = true,
            .fixed_mclk = 0
        };
        
        if (i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL) != ESP_OK) {
            logStatus("I2S driver install failed");
            return false;
        }
        
        if (i2s_set_dac_mode(I2S_DAC_CHANNEL_BOTH_EN) != ESP_OK) {
            logStatus("I2S DAC mode failed");
            return false;
        }
        
    } else {
        // Configuration for external I2S DAC
        i2s_config = {
            .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX),
            .sample_rate = _sampleRate,
            .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
            .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
            .communication_format = (i2s_comm_format_t)(I2S_COMM_FORMAT_STAND_I2S),
            .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
            .dma_buf_count = 8,
            .dma_buf_len = 64,
            .use_apll = false,
            .tx_desc_auto_clear = true,
            .fixed_mclk = 0
        };
        
        if (i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL) != ESP_OK) {
            logStatus("I2S driver install failed");
            return false;
        }
        
        // Set I2S pin configuration
        i2s_pin_config_t pin_config = {
            .bck_io_num = _bckPin,
            .ws_io_num = _wsPin,
            .data_out_num = _dataPin,
            .data_in_num = I2S_PIN_NO_CHANGE
        };
        
        if (i2s_set_pin(I2S_NUM_0, &pin_config) != ESP_OK) {
            logStatus("I2S pin config failed");
            return false;
        }
    }
    
    return true;
}

// Begin Bluetooth audio receiver
bool BT_Audio_Receiver::begin(const char* deviceName) {
    if (_initialized) {
        logStatus("Already initialized");
        return true;
    }
    
    if (deviceName) {
        setDeviceName(deviceName);
    }
    
    // Initialize Bluetooth
    if (!initBluetooth()) {
        return false;
    }
    
    // Initialize A2DP
    if (!initA2DP()) {
        return false;
    }
    
    _initialized = true;
    logStatus("BT Audio Receiver initialized successfully");
    
    return true;
}

// End Bluetooth audio receiver
void BT_Audio_Receiver::end() {
    if (!_initialized) {
        return;
    }
    
    if (_useI2S || _useInternalDAC) {
        i2s_driver_uninstall(I2S_NUM_0);
    }
    
    esp_a2d_sink_deinit();
    esp_bluedroid_disable();
    esp_bluedroid_deinit();
    esp_bt_controller_disable();
    esp_bt_controller_deinit();
    
    _initialized = false;
    _connected = false;
    logStatus("BT Audio Receiver stopped");
}

// Setup I2S for external DAC
bool BT_Audio_Receiver::setupI2S(int bckPin, int wsPin, int dataPin, int sampleRate) {
    _bckPin = bckPin;
    _wsPin = wsPin;
    _dataPin = dataPin;
    _sampleRate = sampleRate;
    _useI2S = true;
    _useInternalDAC = false;
    
    if (_initialized) {
        return initI2S();
    }
    
    return true;
}

// Use internal DAC (GPIO25 and GPIO26)
bool BT_Audio_Receiver::useInternalDAC() {
    _useInternalDAC = true;
    _useI2S = false;
    
    if (_initialized) {
        return initI2S();
    }
    
    return true;
}

// Check if connected
bool BT_Audio_Receiver::isConnected() {
    return _connected;
}

// Set volume (0-127)
void BT_Audio_Receiver::setVolume(uint8_t volume) {
    _volume = volume > 127 ? 127 : volume;
    // Volume control implementation would go here
    // This depends on the specific DAC being used
}

// Get volume
uint8_t BT_Audio_Receiver::getVolume() {
    return _volume;
}

// Register data callback
void BT_Audio_Receiver::onDataReceived(bt_audio_data_callback_t callback) {
    _dataCallback = callback;
}

// Register connection state callback
void BT_Audio_Receiver::onConnectionStateChanged(bt_connection_state_callback_t callback) {
    _connectionCallback = callback;
}

// Register audio state callback
void BT_Audio_Receiver::onAudioStateChanged(bt_audio_state_callback_t callback) {
    _audioStateCallback = callback;
}

// Get Bluetooth MAC address
String BT_Audio_Receiver::getBluetoothAddress() {
    const uint8_t* addr = esp_bt_dev_get_address();
    char macStr[18];
    sprintf(macStr, "%02X:%02X:%02X:%02X:%02X:%02X",
            addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]);
    return String(macStr);
}

// Disconnect
void BT_Audio_Receiver::disconnect() {
    if (_connected) {
        esp_a2d_sink_disconnect(nullptr);
    }
}

// Reconnect
bool BT_Audio_Receiver::reconnect() {
    // Implementation for reconnection logic
    return true;
}

// A2DP callback
void BT_Audio_Receiver::a2dpCallback(esp_a2d_cb_event_t event, esp_a2d_cb_param_t *param) {
    if (!_instance) return;
    
    switch (event) {
        case ESP_A2D_CONNECTION_STATE_EVT:
            _instance->_connected = (param->conn_stat.state == ESP_A2D_CONNECTION_STATE_CONNECTED);
            _instance->logStatus(_instance->_connected ? "Connected" : "Disconnected");
            
            if (_instance->_connectionCallback) {
                _instance->_connectionCallback(param->conn_stat.state, param);
            }
            break;
            
        case ESP_A2D_AUDIO_STATE_EVT:
            if (_instance->_audioStateCallback) {
                _instance->_audioStateCallback(param->audio_stat.state, param);
            }
            break;
            
        case ESP_A2D_AUDIO_CFG_EVT:
            // Audio configuration received
            _instance->logStatus("Audio config received");
            break;
            
        default:
            break;
    }
}

// Data callback
void BT_Audio_Receiver::dataCallback(const uint8_t *data, uint32_t len) {
    if (!_instance) return;
    
    // Write to I2S if configured
    if (_instance->_useI2S || _instance->_useInternalDAC) {
        size_t bytes_written = 0;
        i2s_write(I2S_NUM_0, data, len, &bytes_written, portMAX_DELAY);
    }
    
    // Call user callback if registered
    if (_instance->_dataCallback) {
        _instance->_dataCallback(data, len);
    }
}

// Log status
void BT_Audio_Receiver::logStatus(const char* message) {
    Serial.print("[BT Audio] ");
    Serial.println(message);
}
