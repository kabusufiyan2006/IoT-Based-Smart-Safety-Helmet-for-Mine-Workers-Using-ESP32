#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>

/* ===== WIFI & THINGSBOARD ===== */
#define WIFI_SSID     "YOUR_SSID"
#define WIFI_PASSWORD "YOUR_PASS"
#define THINGSBOARD_SERVER "broker.thingsboard.cloud"
#define THINGSBOARD_TOKEN  "YOUR_DEVICE_TOKEN"

/* ===== PINS ===== */
#define PIN_MQ2     32
#define PIN_PULSE   33
#define PIN_IR      25
#define PIN_TOUCH   4
#define PIN_DHT     21
#define PIN_BUZZER  26

#define DHTTYPE DHT11   // Change to DHT22 if needed

/* ===== TIMING ===== */
const unsigned long SEND_INTERVAL = 3000; // 3s telemetry
const unsigned long SOS_TIME = 3000;      // 3s touch hold for SOS
const unsigned long LOOP_DELAY = 200;     // loop delay ms

/* ===== GLOBALS ===== */
bool systemArmed = false;   // Activated after first touch
int touchBaseline = 0;
unsigned long touchStart = 0;
bool sosActive = false;

/* ===== DHT ===== */
DHT dht(PIN_DHT, DHTTYPE);
unsigned long lastDHT = 0;
float temperature = 0;
float humidity = 0;

/* ===== PULSE SENSOR ===== */
unsigned long lastBeat = 0;
int bpm = 0;
bool pulseAlert = false;
const int PULSE_THRESHOLD = 1800;

/* ===== WIFI & MQTT ===== */
WiFiClient espClient;
PubSubClient client(espClient);

/* ===== CONNECT WIFI ===== */
void connectWiFi() {
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi Connected!");
}

/* ===== CONNECT THINGSBOARD ===== */
void connectTB() {
  while (!client.connected()) {
    Serial.print("Connecting ThingsBoard...");
    if (client.connect("ESP32_Helmet", THINGSBOARD_TOKEN, NULL)) {
      Serial.println(" Connected!");
    } else {
      Serial.print(" Failed rc=");
      Serial.println(client.state());
      delay(2000);
    }
  }
}

/* ===== PULSE SENSOR READ ===== */
void readPulse() {
  int val = analogRead(PIN_PULSE);
  if (val > PULSE_THRESHOLD) {
    unsigned long now = millis();
    if (lastBeat != 0 && (now - lastBeat > 300)) {
      bpm = 60000 / (now - lastBeat);
      if (bpm < 40 || bpm > 180) bpm = 0;
      pulseAlert = (bpm < 50 || bpm > 120);
    }
    lastBeat = now;
  }
}

/* ===== SETUP ===== */
void setup() {
  Serial.begin(115200);

  pinMode(PIN_IR, INPUT);
  pinMode(PIN_BUZZER, OUTPUT);
  digitalWrite(PIN_BUZZER, LOW);

  delay(2000);
  dht.begin();

  connectWiFi();
  client.setServer(THINGSBOARD_SERVER, 1883);

  // Calibrate touch baseline
  touchBaseline = touchRead(PIN_TOUCH);
  Serial.print("Touch baseline: ");
  Serial.println(touchBaseline);
  Serial.println("System ready. Touch the sensor to start telemetry...");
}

/* ===== LOOP ===== */
void loop() {
  if (!client.connected()) connectTB();
  client.loop();

  unsigned long now = millis();

  // ---- TOUCH READ ----
  int touchVal = touchRead(PIN_TOUCH);
  int diff = touchBaseline - touchVal;
  bool touched = (diff > 20); // threshold

  // ---- SYSTEM ARM (first touch) ----
  if (touched && !systemArmed) {
    systemArmed = true;
    Serial.println("✅ SYSTEM ACTIVATED (FIRST TOUCH)");
  }

  // ---- SOS LOGIC ----
  if (systemArmed) {
    if (touched) {
      if (touchStart == 0) touchStart = now;
      if (!sosActive && (now - touchStart >= SOS_TIME)) {
        sosActive = true;
        digitalWrite(PIN_BUZZER, HIGH);
        client.publish("v1/devices/me/telemetry", "{\"sos\":1}");
        Serial.println("🚨 SOS ACTIVATED!");
      }
    } else {
      touchStart = 0;
      if (sosActive) {
        sosActive = false;
        digitalWrite(PIN_BUZZER, LOW);
        client.publish("v1/devices/me/telemetry", "{\"sos\":0}");
        Serial.println("✅ SOS CLEARED");
      }
    }
  }

  // ---- SENSOR READINGS (always print to Serial) ----
  int mq2Val  = analogRead(PIN_MQ2);
  int irVal   = digitalRead(PIN_IR);

  // DHT every 2s
  if (now - lastDHT >= 2000) {
    lastDHT = now;
    float t = dht.readTemperature();
    float h = dht.readHumidity();
    if (!isnan(t) && !isnan(h)) {
      temperature = t;
      humidity = h;
    }
  }

  // Pulse sensor
  readPulse();

  // ---- SERIAL MONITOR OUTPUT ----
  Serial.print("Touch: "); Serial.print(touchVal);
  Serial.print(" | MQ2: "); Serial.print(mq2Val);
  Serial.print(" | Temp: "); Serial.print(temperature);
  Serial.print(" | Hum: "); Serial.print(humidity);
  Serial.print(" | IR: "); Serial.print(irVal);
  Serial.print(" | BPM: "); Serial.print(bpm);
  Serial.print(" | PulseAlert: "); Serial.print(pulseAlert);
  Serial.print(" | SOS: "); Serial.println(sosActive);

  // ---- SEND TO THINGSBOARD (only if system armed) ----
  if (systemArmed && (now % SEND_INTERVAL < LOOP_DELAY)) {
    String payload = "{";
    payload += "\"mq2\":" + String(mq2Val) + ",";
    payload += "\"temperature\":" + String(temperature,1) + ",";
    payload += "\"humidity\":" + String(humidity,1) + ",";
    payload += "\"ir\":" + String(irVal) + ",";
    payload += "\"bpm\":" + String(bpm) + ",";
    payload += "\"pulse_alert\":" + String(pulseAlert) + ",";
    payload += "\"sos\":" + String(sosActive ? 1 : 0);
    payload += "}";
    client.publish("v1/devices/me/telemetry", payload.c_str());
  }

  delay(LOOP_DELAY);
}