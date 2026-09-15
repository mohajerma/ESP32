/* Simple ESP8266 hardware test. */

const uint8_t led_pin = LED_BUILTIN;
unsigned long last_report = 0;
bool led_state = HIGH;

void setup() {
  Serial.begin(115200);
  delay(1000);

  pinMode(led_pin, OUTPUT);
  digitalWrite(led_pin, led_state);
  Serial.println();
  Serial.println("ESP8266 test started");
  Serial.println("The built-in LED should blink every second.");
}

void loop() {
  if (millis() - last_report >= 1000) {
    last_report = millis();
    led_state = !led_state;
    digitalWrite(led_pin, led_state);
    Serial.print("ESP8266 is running, uptime: ");
    Serial.print(last_report / 1000);
    Serial.println(" s");
  }
}
