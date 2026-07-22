/*
 * Bluetooth BLE Server Module
 * Handles Bluetooth Low Energy communication for LED control
 */

#ifndef BLE_SERVER_H
#define BLE_SERVER_H

#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

extern BLEServer* pBLEServer;
extern BLECharacteristic* pCharacteristic;
extern BLECharacteristic* pTxCharacteristic;
extern bool deviceConnected;
extern String currentMode;
extern String currentText;
extern int brightness;
extern int scrollSpeed;

// BLE Server callbacks
class MyServerCallbacks: public BLEServerCallbacks {
  void onConnect(BLEServer* pServer) {
    deviceConnected = true;
    Serial.println("BLE Client connected");
  }
  
  void onDisconnect(BLEServer* pServer) {
    deviceConnected = false;
    Serial.println("BLE Client disconnected");
    // Restart advertising
    BLEDevice::startAdvertising();
  }
};

// BLE Characteristic callbacks
class MyCallbacks: public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristic) {
    String command = pCharacteristic->getValue().c_str();
    
    if (command.length() > 0) {
      Serial.println("BLE received: " + command);
      
      // Parse command format: "CMD:VALUE"
      int colonIndex = command.indexOf(':');
      if (colonIndex > 0) {
        String cmd = command.substring(0, colonIndex);
        String val = command.substring(colonIndex + 1);
        
        cmd.toUpperCase();
        
        if (cmd == "MODE") {
          setMode(val);
        }
        else if (cmd == "TEXT") {
          setText(val);
        }
        else if (cmd == "BRIGHTNESS") {
          setBrightness(val.toInt());
        }
        else if (cmd == "SPEED") {
          setScrollSpeed(val.toInt());
        }
        else if (cmd == "COLOR") {
          // Parse color in format R,G,B
          int comma1 = val.indexOf(',');
          int comma2 = val.indexOf(',', comma1 + 1);
          if (comma1 > 0 && comma2 > 0) {
            int r = val.substring(0, comma1).toInt();
            int g = val.substring(comma1 + 1, comma2).toInt();
            int b = val.substring(comma2 + 1).toInt();
            // Set custom color (you can add this function)
            Serial.printf("Color set to RGB(%d, %d, %d)\n", r, g, b);
          }
        }
      }
    }
  }
};

// Initialize BLE
void initBLE() {
  Serial.println("Initializing Bluetooth BLE...");
  
  // Create BLE Device
  BLEDevice::init(BLE_DEVICE_NAME);
  
  // Create BLE Server
  pBLEServer = BLEDevice::createServer();
  pBLEServer->setCallbacks(new MyServerCallbacks());
  
  // Create BLE Service (Nordic UART Service)
  BLEService *pService = pBLEServer->createService(SERVICE_UUID);
  
  // Create RX Characteristic (for receiving data from phone)
  pCharacteristic = pService->createCharacteristic(
    CHARACTERISTIC_UUID_RX,
    BLECharacteristic::PROPERTY_WRITE |
    BLECharacteristic::PROPERTY_WRITE_NR
  );
  pCharacteristic->setCallbacks(new MyCallbacks());
  
  // Create TX Characteristic (for sending data to phone)
  pTxCharacteristic = pService->createCharacteristic(
    CHARACTERISTIC_UUID_TX,
    BLECharacteristic::PROPERTY_READ |
    BLECharacteristic::PROPERTY_NOTIFY
  );
  pTxCharacteristic->addDescriptor(new BLE2902());
  
  // Start the service
  pService->start();
  
  // Start advertising
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06);
  pAdvertising->setMinPreferred(0x12);
  BLEDevice::startAdvertising();
  
  Serial.println("BLE initialized!");
  Serial.println("Bluetooth device name: " + String(BLE_DEVICE_NAME));
  Serial.println("Waiting for client connection...");
  Serial.println("\nBLE Commands format: CMD:VALUE");
  Serial.println("  MODE:rainbow");
  Serial.println("  TEXT:Hello World");
  Serial.println("  BRIGHTNESS:200");
  Serial.println("  SPEED:100");
  Serial.println("  COLOR:255,0,0");
}

// Handle BLE updates
void handleBLE() {
  // Check if device is connected
  if (deviceConnected) {
    // Send status update (optional)
    // You can periodically send status updates to the connected device
    static unsigned long lastNotify = 0;
    if (millis() - lastNotify > 5000) { // Every 5 seconds
      String status = "STATUS:mode=" + currentMode + 
                     ",brightness=" + String(brightness) +
                     ",speed=" + String(scrollSpeed);
      pTxCharacteristic->setValue(status.c_str());
      pTxCharacteristic->notify();
      lastNotify = millis();
    }
  }
}

#endif // BLE_SERVER_H
