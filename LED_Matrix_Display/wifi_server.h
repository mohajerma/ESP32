/*
 * WiFi Web Server Module
 * Handles WiFi connection and web interface for LED control
 */

#ifndef WIFI_SERVER_H
#define WIFI_SERVER_H

#include <WiFi.h>
#include <WebServer.h>

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
    
    function showStatus(message) {
      document.getElementById('status').innerText = message;
      setTimeout(() => {
        document.getElementById('status').innerText = 'Ready';
      }, 2000);
    }
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
  String json = "{";
  json += "\"mode\":\"" + currentMode + "\",";
  json += "\"text\":\"" + currentText + "\",";
  json += "\"brightness\":" + String(brightness) + ",";
  json += "\"speed\":" + String(scrollSpeed);
  json += "}";
  
  webServer.send(200, "application/json", json);
}

#endif // WIFI_SERVER_H
