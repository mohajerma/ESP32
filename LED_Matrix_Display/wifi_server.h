/*
 * WiFi Web Server Module
 * Handles WiFi connection and web interface for LED control
 */

#ifndef WIFI_SERVER_H
#define WIFI_SERVER_H

#include <WiFi.h>
#include <WebServer.h>
#include <Preferences.h>

extern WebServer webServer;
extern String currentMode;
extern String currentText;
extern int brightness;
extern int scrollSpeed;

// Function declarations
void initWiFi();
void setupWebServer();
void handleRoot();
void handleSetMode();
void handleSetText();
void handleSetBrightness();
void handleSetSpeed();
void handleGetStatus();
void handleSetMatrixSize();
void applyMatrixConfiguration(int width, int height);

// Initialize WiFi connection
void initWiFi() {
  Serial.println("Initializing WiFi...");
  
  #if AP_MODE
    // Access Point mode
    WiFi.mode(WIFI_AP);
    delay(100);
    
    // Configure and start Access Point
    // Parameters: SSID, Password, Channel (1-13), Hide SSID (false), Max Connections (4)
    bool result = WiFi.softAP(AP_SSID, AP_PASSWORD, 1, false, 4);
    
    if (result) {
      delay(500); // Wait for AP to start
      IPAddress IP = WiFi.softAPIP();
      Serial.println("\n=== Access Point Started ===");
      Serial.print("Network Name (SSID): ");
      Serial.println(AP_SSID);
      Serial.print("Password: ");
      Serial.println(AP_PASSWORD);
      Serial.print("IP address: ");
      Serial.println(IP);
      Serial.println("============================\n");
    } else {
      Serial.println("Failed to start Access Point!");
    }
  #else
    // Station mode - connect to existing WiFi
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    
    Serial.print("Connecting to WiFi");
    unsigned long startAttemptTime = millis();
    
    while (WiFi.status() != WL_CONNECTED && 
           millis() - startAttemptTime < WIFI_TIMEOUT) {
      delay(500);
      Serial.print(".");
    }
    
    if (WiFi.status() == WL_CONNECTED) {
      Serial.println("\nWiFi connected!");
      Serial.print("IP address: ");
      Serial.println(WiFi.localIP());
      Serial.print("Signal strength: ");
      Serial.print(WiFi.RSSI());
      Serial.println(" dBm");
    } else {
      Serial.println("\nWiFi connection failed!");
      Serial.println("Starting in offline mode...");
    }
  #endif
}

// Setup web server routes and handlers
void setupWebServer() {
  webServer.on("/", handleRoot);
  webServer.on("/setMode", handleSetMode);
  webServer.on("/setText", handleSetText);
  webServer.on("/setBrightness", handleSetBrightness);
  webServer.on("/setSpeed", handleSetSpeed);
  webServer.on("/status", handleGetStatus);
  webServer.on("/setMatrixSize", handleSetMatrixSize);
  
  webServer.begin();
  Serial.println("Web server started!");
  
  #if AP_MODE
    Serial.println("Access the control panel at: http://192.168.4.1");
  #else
    Serial.println("Access the control panel at: http://" + WiFi.localIP().toString());
  #endif
}

// Main web interface
void handleRoot() {
  String html = R"=====(
<!DOCTYPE html>
<html>
<head>
  <meta charset='UTF-8'>
  <meta name='viewport' content='width=device-width, initial-scale=1.0'>
  <title>LED Matrix Display</title>
  <style>
    * { margin: 0; padding: 0; box-sizing: border-box; }
    body { 
      font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif; 
      background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
      color: #fff;
      padding: 20px;
    }
    .container { 
      max-width: 600px; 
      margin: 0 auto; 
      background: rgba(255,255,255,0.1);
      backdrop-filter: blur(10px);
      border-radius: 20px;
      padding: 30px;
      box-shadow: 0 8px 32px 0 rgba(31, 38, 135, 0.37);
    }
    h1 { 
      text-align: center; 
      margin-bottom: 30px;
      font-size: 2em;
      text-shadow: 2px 2px 4px rgba(0,0,0,0.3);
    }
    .control-group { 
      margin-bottom: 25px;
      background: rgba(255,255,255,0.05);
      padding: 20px;
      border-radius: 15px;
    }
    label { 
      display: block; 
      margin-bottom: 10px;
      font-weight: 600;
      font-size: 1.1em;
    }
    select, input[type='text'], input[type='range'] { 
      width: 100%; 
      padding: 12px;
      border: none;
      border-radius: 10px;
      font-size: 16px;
      background: rgba(255,255,255,0.9);
      color: #333;
    }
    button { 
      width: 100%;
      padding: 15px;
      background: linear-gradient(135deg, #f093fb 0%, #f5576c 100%);
      color: white;
      border: none;
      border-radius: 10px;
      font-size: 18px;
      font-weight: 600;
      cursor: pointer;
      transition: transform 0.2s;
      margin-top: 10px;
    }
    button:hover { transform: scale(1.05); }
    button:active { transform: scale(0.95); }
    .value-display { 
      text-align: center;
      font-size: 1.2em;
      margin-top: 10px;
      padding: 10px;
      background: rgba(255,255,255,0.1);
      border-radius: 8px;
    }
    .status { 
      text-align: center;
      padding: 15px;
      background: rgba(0,255,0,0.2);
      border-radius: 10px;
      margin-top: 20px;
      font-weight: 600;
    }
  </style>
</head>
<body>
  <div class='container'>
    <h1>🎨 LED Matrix Display</h1>
    
    <div class='control-group'>
      <label>Display Mode</label>
      <select id='mode' onchange='setMode()'>
        <option value='rainbow'>🌈 Rainbow</option>
        <option value='scroll_text'>📝 Scrolling Text</option>
        <option value='color_wipe'>🎨 Color Wipe</option>
        <option value='theater_chase'>🎭 Theater Chase</option>
        <option value='fire'>🔥 Fire</option>
        <option value='confetti'>🎊 Confetti</option>
        <option value='solid'>💡 Solid White</option>
        <option value='off'>⚫ Off</option>
      </select>
    </div>
    
    <div class='control-group'>
      <label>Scrolling Text</label>
      <input type='text' id='text' placeholder='Enter text to display'>
      <button onclick='setText()'>Update Text</button>
    </div>
    
    <div class='control-group'>
      <label>Brightness: <span id='brightnessValue'>128</span></label>
      <input type='range' id='brightness' min='0' max='255' value='128' 
             oninput='updateBrightnessValue()' onchange='setBrightness()'>
    </div>
    
    <div class='control-group'>
      <label>Animation Speed: <span id='speedValue'>50ms</span></label>
      <input type='range' id='speed' min='10' max='500' value='50' 
             oninput='updateSpeedValue()' onchange='setSpeed()'>
    </div>
    
    <div class='control-group'>
      <label>Matrix Configuration</label>
      <div style='display: grid; grid-template-columns: 1fr 1fr 1fr; gap: 10px; margin-top: 10px;'>
        <div>
          <label style='font-size: 0.9em;'>Width</label>
          <input type='number' id='matrixWidth' min='1' max='100' value='10' 
                 oninput='updateTotalLEDs()' style='margin-top: 5px;'>
        </div>
        <div>
          <label style='font-size: 0.9em;'>Height</label>
          <input type='number' id='matrixHeight' min='1' max='100' value='6' 
                 oninput='updateTotalLEDs()' style='margin-top: 5px;'>
        </div>
        <button onclick='setMatrixSize()' style='margin-top: 22px;'>Apply</button>
      </div>
      <div style='font-size: 0.9em; margin-top: 10px; padding: 8px; background: rgba(255,255,255,0.15); border-radius: 5px; text-align: center;'>
        Total LEDs: <strong><span id='totalLEDs'>60</span></strong>
      </div>
      <div style='font-size: 0.85em; margin-top: 10px; padding: 8px; background: rgba(255,255,255,0.1); border-radius: 5px;'>
        ℹ️ Changes apply immediately (no restart needed)
      </div>
    </div>
    
    <div id='status' class='status'>Ready</div>
  </div>
  
  <script>
    function setMode() {
      let mode = document.getElementById('mode').value;
      fetch('/setMode?mode=' + mode)
        .then(response => response.text())
        .then(data => showStatus('Mode: ' + mode));
    }
    
    function setText() {
      let text = document.getElementById('text').value;
      fetch('/setText?text=' + encodeURIComponent(text))
        .then(response => response.text())
        .then(data => showStatus('Text updated'));
    }
    
    function setBrightness() {
      let brightness = document.getElementById('brightness').value;
      fetch('/setBrightness?value=' + brightness)
        .then(response => response.text())
        .then(data => showStatus('Brightness: ' + brightness));
    }
    
    function setSpeed() {
      let speed = document.getElementById('speed').value;
      fetch('/setSpeed?value=' + speed)
        .then(response => response.text())
        .then(data => showStatus('Speed: ' + speed + 'ms'));
    }
    
    function updateBrightnessValue() {
      document.getElementById('brightnessValue').innerText = 
        document.getElementById('brightness').value;
    }
    
    function updateSpeedValue() {
      document.getElementById('speedValue').innerText = 
        document.getElementById('speed').value + 'ms';
    }
    
    function updateTotalLEDs() {
      let width = parseInt(document.getElementById('matrixWidth').value) || 0;
      let height = parseInt(document.getElementById('matrixHeight').value) || 0;
      let total = width * height;
      document.getElementById('totalLEDs').innerText = total;
    }
    
    function setMatrixSize() {
      let width = document.getElementById('matrixWidth').value;
      let height = document.getElementById('matrixHeight').value;
      let total = width * height;
      fetch('/setMatrixSize?width=' + width + '&height=' + height)
        .then(response => response.text())
        .then(data => showStatus('Matrix: ' + width + 'x' + height + ' (' + total + ' LEDs) - Applied!'));
    }
    
    function showStatus(message) {
      document.getElementById('status').innerText = message;
      setTimeout(() => {
        document.getElementById('status').innerText = 'Ready';
      }, 2000);
    }
    
    // Load current configuration on page load
    function loadConfig() {
      fetch('/status')
        .then(response => response.json())
        .then(data => {
          // Update matrix configuration
          if (data.matrixWidth) {
            document.getElementById('matrixWidth').value = data.matrixWidth;
          }
          if (data.matrixHeight) {
            document.getElementById('matrixHeight').value = data.matrixHeight;
          }
          updateTotalLEDs();
          
          // Update other settings
          if (data.brightness) {
            document.getElementById('brightness').value = data.brightness;
            updateBrightnessValue();
          }
          if (data.speed) {
            document.getElementById('speed').value = data.speed;
            updateSpeedValue();
          }
          if (data.text) {
            document.getElementById('text').value = data.text;
          }
          
          console.log('Configuration loaded:', data);
        })
        .catch(err => console.error('Failed to load config:', err));
    }
    
    // Load configuration when page loads
    window.addEventListener('DOMContentLoaded', loadConfig);
  </script>
</body>
</html>
  )=====";
  
  webServer.send(200, "text/html; charset=UTF-8", html);
}

// Handle mode change
void handleSetMode() {
  if (webServer.hasArg("mode")) {
    String mode = webServer.arg("mode");
    setMode(mode);
    webServer.send(200, "text/plain", "OK");
  } else {
    webServer.send(400, "text/plain", "Missing parameter");
  }
}

// Handle text update
void handleSetText() {
  if (webServer.hasArg("text")) {
    String text = webServer.arg("text");
    setText(text);
    webServer.send(200, "text/plain", "OK");
  } else {
    webServer.send(400, "text/plain", "Missing parameter");
  }
}

// Handle brightness change
void handleSetBrightness() {
  if (webServer.hasArg("value")) {
    int value = webServer.arg("value").toInt();
    setBrightness(value);
    webServer.send(200, "text/plain", "OK");
  } else {
    webServer.send(400, "text/plain", "Missing parameter");
  }
}

// Handle speed change
void handleSetSpeed() {
  if (webServer.hasArg("value")) {
    int value = webServer.arg("value").toInt();
    setScrollSpeed(value);
    webServer.send(200, "text/plain", "OK");
  } else {
    webServer.send(400, "text/plain", "Missing parameter");
  }
}

// Handle status request
void handleGetStatus() {
  // Try to load saved configuration from NVS
  Preferences prefs;
  prefs.begin("led-config", true); // Read-only
  
  int savedWidth = prefs.getInt("matrix_w", 0);
  int savedHeight = prefs.getInt("matrix_h", 0);
  int savedNumLEDs = prefs.getInt("num_leds", 0);
  
  prefs.end();
  
  // If no saved config, use compiled values
  int currentWidth = (savedWidth > 0) ? savedWidth : MATRIX_WIDTH;
  int currentHeight = (savedHeight > 0) ? savedHeight : MATRIX_HEIGHT;
  int currentNumLEDs = (savedNumLEDs > 0) ? savedNumLEDs : NUM_LEDS;
  
  String json = "{";
  json += "\"mode\":\"" + currentMode + "\",";
  json += "\"text\":\"" + currentText + "\",";
  json += "\"brightness\":" + String(brightness) + ",";
  json += "\"speed\":" + String(scrollSpeed) + ",";
  json += "\"matrixWidth\":" + String(currentWidth) + ",";
  json += "\"matrixHeight\":" + String(currentHeight) + ",";
  json += "\"numLEDs\":" + String(currentNumLEDs);
  json += "}";
  
  webServer.send(200, "application/json", json);
}

// Handle matrix size change
void handleSetMatrixSize() {
  if (webServer.hasArg("width") && webServer.hasArg("height")) {
    int width = webServer.arg("width").toInt();
    int height = webServer.arg("height").toInt();
    
    // Validate input
    if (width < 1 || width > 100 || height < 1 || height > 100) {
      webServer.send(400, "text/plain", "Invalid matrix dimensions (1-100)");
      return;
    }
    
    // Calculate total LEDs
    int totalLEDs = width * height;
    
    // Validate total against MAX_LEDS
    if (totalLEDs > MAX_LEDS) {
      String msg = "Total LEDs (" + String(totalLEDs) + ") exceeds MAX_LEDS (" + String(MAX_LEDS) + ")";
      webServer.send(400, "text/plain", msg);
      return;
    }
    
    // Save to preferences (will require restart to take effect)
    Preferences prefs;
    prefs.begin("led-config", false);
    prefs.putInt("num_leds", totalLEDs);  // Calculated from width × height
    prefs.putInt("matrix_w", width);
    prefs.putInt("matrix_h", height);
    prefs.end();
    
    Serial.print("Matrix size updated to: ");
    Serial.print(width);
    Serial.print("x");
    Serial.print(height);
    Serial.print(" = ");
    Serial.print(totalLEDs);
    Serial.println(" LEDs");
    Serial.println("Applying configuration immediately...");
    
    // Apply the new configuration without restart
    applyMatrixConfiguration(width, height);
    
    webServer.send(200, "text/plain", "OK - Applied");
  } else {
    webServer.send(400, "text/plain", "Missing parameters");
  }
}

// Apply matrix configuration changes immediately (hot-swap)
void applyMatrixConfiguration(int width, int height) {
  extern int activeLEDCount;
  extern int activeMatrixWidth;
  extern int activeMatrixHeight;
  extern CRGB leds[];
  
  // Update active configuration
  activeMatrixWidth = width;
  activeMatrixHeight = height;
  activeLEDCount = width * height;
  
  // Clear all LEDs
  FastLED.clear();
  FastLED.show();
  
  // Reinitialize FastLED with new LED count
  #ifdef LED_PIN
    FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, activeLEDCount);
  #else
    FastLED.addLeds<LED_TYPE, LED_DATA_PIN, LED_CLOCK_PIN, COLOR_ORDER>(leds, activeLEDCount);
  #endif
  
  // Restore brightness
  extern int brightness;
  FastLED.setBrightness(brightness);
  
  Serial.println("✓ Configuration applied successfully!");
  Serial.printf("  Active LEDs: %d (%dx%d)\n", activeLEDCount, activeMatrixWidth, activeMatrixHeight);
  
  // Brief confirmation flash
  for(int i = 0; i < min(10, activeLEDCount); i++) {
    leds[i] = CRGB::Green;
  }
  FastLED.show();
  delay(200);
  FastLED.clear();
  FastLED.show();
}

#endif // WIFI_SERVER_H
