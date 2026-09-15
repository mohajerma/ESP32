/*
 * Stepper Web Controller
 *
 * Controls a NEMA 17 through an A4988/DRV8825-style STEP/DIR driver.
 * Works with ESP8266 and ESP32 Arduino cores without external libraries.
 */

#if defined(ESP8266)
  #include <ESP8266WiFi.h>
  #include <ESP8266WebServer.h>
  using WebServerType = ESP8266WebServer;
#else
  #include <WiFi.h>
  #include <WebServer.h>
  using WebServerType = WebServer;
#endif

// Set USE_ACCESS_POINT to 0 to join an existing WiFi network instead.
#define USE_ACCESS_POINT 1
#define WIFI_SSID "StepperBench"
#define WIFI_PASSWORD "stepper123"
#define STA_SSID "your-wifi-name"
#define STA_PASSWORD "your-wifi-password"
#define AP_CHANNEL 6

// GPIO numbers, not board labels. These are shared by common NodeMCU and ESP32 DevKit boards.
const uint8_t STEP_PIN = 5;       // ESP8266 D1, ESP32 GPIO5
const uint8_t DIR_PIN = 4;        // ESP8266 D2, ESP32 GPIO4
const uint8_t ENABLE_PIN = 16;    // ESP8266 D0, ESP32 GPIO16; active LOW
const uint8_t LIMIT_MIN_PIN = 14; // Optional switch input; ESP8266 D5, ESP32 GPIO14
const uint8_t LIMIT_MAX_PIN = 12; // Optional switch input; ESP8266 D6, ESP32 GPIO12
#define USE_LIMIT_SWITCHES 0
#define LIMIT_ACTIVE_LOW 1

const uint32_t DEFAULT_SPEED = 400;
const uint32_t MIN_SPEED = 10;
const uint32_t MAX_SPEED = 2000;
const uint16_t STEP_PULSE_US = 4;
const uint16_t JOG_STEPS = 200;

WebServerType server(80);

bool driverEnabled = false;
bool stepHigh = false;
int32_t positionSteps = 0;
int32_t stepsRemaining = 0;
uint32_t stepsPerSecond = DEFAULT_SPEED;
uint32_t nextStepMicros = 0;
uint32_t pulseEndMicros = 0;

const char INDEX_HTML[] PROGMEM = R"HTML(
<!doctype html>
<html lang="en">
<head>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width,initial-scale=1">
<title>Stepper Bench</title>
<style>
:root{color-scheme:light;--ink:#16242b;--muted:#65757b;--paper:#f4f1e8;--panel:#fffdf7;--accent:#d6532f;--line:#d8d4c8;--ok:#2c8064}
*{box-sizing:border-box}body{margin:0;background:radial-gradient(circle at 15% 0,#fff8d5 0,transparent 34%),linear-gradient(135deg,#f4f1e8,#e4ece7);color:var(--ink);font:16px/1.45 Georgia,serif;min-height:100vh;padding:22px}
main{max-width:780px;margin:auto}.kicker{color:var(--accent);font:700 12px/1.2 Arial,sans-serif;letter-spacing:2px;text-transform:uppercase}h1{font-size:clamp(2.2rem,7vw,4.7rem);line-height:.95;margin:8px 0 12px;max-width:600px}p{color:var(--muted);max-width:620px}.grid{display:grid;grid-template-columns:repeat(auto-fit,minmax(220px,1fr));gap:14px;margin-top:24px}.panel{background:rgba(255,253,247,.9);border:1px solid var(--line);padding:18px;box-shadow:0 10px 30px rgba(39,52,45,.08)}.panel h2{font-size:1rem;margin:0 0 14px;font-family:Arial,sans-serif}.readout{font:700 2rem/1 Arial,sans-serif}.unit{color:var(--muted);font:12px Arial,sans-serif}.row{display:flex;gap:8px;flex-wrap:wrap;align-items:center}button,input{font:inherit}button{border:1px solid var(--ink);background:var(--ink);color:#fffdf7;padding:10px 14px;cursor:pointer}button.secondary{background:transparent;color:var(--ink)}button.danger{background:var(--accent);border-color:var(--accent)}button:active{transform:translateY(1px)}input[type=number]{width:110px;border:1px solid var(--line);padding:9px;background:#fff}.range{width:100%;accent-color:var(--accent)}label{display:block;font:700 12px Arial,sans-serif;text-transform:uppercase;letter-spacing:.7px;margin:14px 0 6px}.status{margin-top:18px;border-left:4px solid var(--ok);padding:9px 12px;background:rgba(44,128,100,.1);font-family:Arial,sans-serif;font-size:13px}.small{font:12px Arial,sans-serif;color:var(--muted)}
</style>
</head>
<body>
<main>
<div class="kicker">WiFi motion bench</div>
<h1>Stepper, at your command.</h1>
<p>Test direction, speed, enable state, and repeatable moves without blocking the controller loop.</p>
<div class="grid">
<section class="panel"><h2>Position</h2><div class="readout" id="position">0</div><span class="unit">steps from boot</span><div class="status" id="state">Connecting...</div></section>
<section class="panel"><h2>Driver</h2><div class="row"><button id="enable" onclick="setEnable(1)">Enable</button><button class="secondary" onclick="setEnable(0)">Disable</button></div><label for="speed">Speed: <span id="speedValue">400</span> steps/s</label><input class="range" id="speed" type="range" min="10" max="2000" value="400" oninput="speedValue.textContent=this.value" onchange="setSpeed()"></section>
<section class="panel"><h2>Jog</h2><div class="row"><button class="secondary" onclick="move(-200)">-200</button><button onclick="move(200)">+200</button><button class="danger" onclick="stopMotor()">Stop</button></div><label for="distance">Move distance</label><div class="row"><input id="distance" type="number" value="1600" step="1"><button onclick="move(Number(distance.value))">Move +</button><button class="secondary" onclick="move(-Number(distance.value))">Move -</button></div></section>
<section class="panel"><h2>Tests</h2><div class="row"><button onclick="test('cycle')">Cycle 1 rev</button><button class="secondary" onclick="test('direction')">Direction</button><button class="secondary" onclick="test('pulse')">Pulse test</button></div><p class="small">Assumes 200 full steps per revolution. Set your driver microstep switch before testing.</p></section>
</div>
</main>
<script>
const state=document.getElementById('state'),position=document.getElementById('position');
async function api(path){const response=await fetch(path);if(!response.ok)throw Error(await response.text());return response.json()}
async function refresh(){try{const s=await api('/api/status');position.textContent=s.position;state.textContent=s.moving?'Moving · '+s.remaining+' remaining':(s.enabled?'Ready · driver enabled':'Idle · driver disabled');document.getElementById('enable').textContent=s.enabled?'Enabled':'Enable'}catch(e){state.textContent='Connection lost'}}
async function move(steps){await api('/api/move?steps='+steps+'&speed='+document.getElementById('speed').value);refresh()}
async function setEnable(value){await api('/api/enable?value='+value);refresh()}
async function setSpeed(){await api('/api/speed?value='+document.getElementById('speed').value);refresh()}
async function stopMotor(){await api('/api/stop');refresh()}
async function test(type){await api('/api/test?type='+type);refresh()}
setInterval(refresh,500);refresh();
</script>
</body>
</html>
)HTML";

bool limitTriggered(uint8_t pin) {
#if USE_LIMIT_SWITCHES
  return digitalRead(pin) == (LIMIT_ACTIVE_LOW ? LOW : HIGH);
#else
  (void)pin;
  return false;
#endif
}

void setDriverEnabled(bool enabled) {
  driverEnabled = enabled;
  digitalWrite(ENABLE_PIN, enabled ? LOW : HIGH);
  if (!enabled) {
    stepsRemaining = 0;
    stepHigh = false;
    digitalWrite(STEP_PIN, LOW);
  }
}

void startMove(int32_t steps, uint32_t speed) {
  if (steps == 0) return;
  stepsPerSecond = constrain(speed, MIN_SPEED, MAX_SPEED);
  setDriverEnabled(true);
  digitalWrite(DIR_PIN, steps > 0 ? HIGH : LOW);
  stepsRemaining = abs(steps);
  nextStepMicros = micros();
}

void stopMove() {
  stepsRemaining = 0;
  stepHigh = false;
  digitalWrite(STEP_PIN, LOW);
}

void serviceMotion() {
  if (!driverEnabled || stepsRemaining <= 0) return;
  uint32_t now = micros();
  uint32_t interval = 1000000UL / stepsPerSecond;

  if (stepHigh) {
    if ((int32_t)(now - pulseEndMicros) >= 0) {
      digitalWrite(STEP_PIN, LOW);
      stepHigh = false;
      nextStepMicros = now + (interval > STEP_PULSE_US ? interval - STEP_PULSE_US : 1);
    }
    return;
  }

  if ((int32_t)(now - nextStepMicros) < 0) return;
  bool movingPositive = digitalRead(DIR_PIN) == HIGH;
  if ((movingPositive && limitTriggered(LIMIT_MAX_PIN)) || (!movingPositive && limitTriggered(LIMIT_MIN_PIN))) {
    stopMove();
    return;
  }
  digitalWrite(STEP_PIN, HIGH);
  stepHigh = true;
  pulseEndMicros = now + STEP_PULSE_US;
  stepsRemaining--;
  positionSteps += movingPositive ? 1 : -1;
}

void sendStatus() {
  String json = "{\"enabled\":" + String(driverEnabled ? "true" : "false") +
                ",\"moving\":" + String(stepsRemaining > 0 ? "true" : "false") +
                ",\"position\":" + String(positionSteps) +
                ",\"remaining\":" + String(stepsRemaining) +
                ",\"speed\":" + String(stepsPerSecond) + "}";
  server.send(200, "application/json", json);
}

void handleMove() {
  if (!server.hasArg("steps")) { server.send(400, "text/plain", "steps is required"); return; }
  startMove(server.arg("steps").toInt(), server.arg("speed").toInt());
  sendStatus();
}

void handleTest() {
  String type = server.arg("type");
  if (type == "cycle") startMove(200, 400);
  else if (type == "direction") startMove(100, 150);
  else if (type == "pulse") startMove(20, 20);
  else { server.send(400, "text/plain", "unknown test"); return; }
  sendStatus();
}

void handleEnable() {
  setDriverEnabled(server.arg("value") != "0");
  sendStatus();
}

void handleSpeed() {
  stepsPerSecond = constrain(server.arg("value").toInt(), MIN_SPEED, MAX_SPEED);
  sendStatus();
}

void connectWiFi() {
#if USE_ACCESS_POINT
  WiFi.mode(WIFI_AP);
  WiFi.softAP(WIFI_SSID, WIFI_PASSWORD, AP_CHANNEL);
  Serial.print("Access point: "); Serial.println(WIFI_SSID);
  Serial.print("Open: http://"); Serial.println(WiFi.softAPIP());
#else
  WiFi.mode(WIFI_STA);
  WiFi.begin(STA_SSID, STA_PASSWORD);
  Serial.print("Connecting to WiFi");
  uint32_t started = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - started < 15000) { delay(250); Serial.print('.'); }
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println(); Serial.print("Open: http://"); Serial.println(WiFi.localIP());
  } else {
    Serial.println("\nWiFi failed; starting access point.");
    WiFi.mode(WIFI_AP); WiFi.softAP(WIFI_SSID, WIFI_PASSWORD, AP_CHANNEL);
    Serial.print("Open: http://"); Serial.println(WiFi.softAPIP());
  }
#endif
}

void setupWebServer() {
  server.on("/", HTTP_GET, []() { server.send(200, "text/html", INDEX_HTML); });
  server.on("/api/status", HTTP_GET, sendStatus);
  server.on("/api/move", HTTP_GET, handleMove);
  server.on("/api/stop", HTTP_GET, []() { stopMove(); sendStatus(); });
  server.on("/api/enable", HTTP_GET, handleEnable);
  server.on("/api/speed", HTTP_GET, handleSpeed);
  server.on("/api/test", HTTP_GET, handleTest);
  server.onNotFound([]() { server.send(404, "text/plain", "Not found"); });
  server.begin();
}

void setup() {
  Serial.begin(115200);
  pinMode(STEP_PIN, OUTPUT);
  pinMode(DIR_PIN, OUTPUT);
  pinMode(ENABLE_PIN, OUTPUT);
#if USE_LIMIT_SWITCHES
  pinMode(LIMIT_MIN_PIN, INPUT_PULLUP);
  pinMode(LIMIT_MAX_PIN, INPUT_PULLUP);
#endif
  digitalWrite(STEP_PIN, LOW);
  digitalWrite(DIR_PIN, LOW);
  setDriverEnabled(false);
  connectWiFi();
  setupWebServer();
  Serial.println("Stepper web controller ready");
}

void loop() {
  server.handleClient();
  serviceMotion();
  yield();
}
