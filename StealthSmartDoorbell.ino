#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "StealthSmartDoorbell_config.h"
#include "StealthSmartDoorbell_secrets.h"

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

static long ssdTriggered = 0;
static long ssdButtonPressed = 0;
static long ssdLedOn = 1;
static long ssdLedOff = 0;
static long ssdMqttConnect = 0;
static bool ssdMqttFirstConnectAttempt = true;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(SSD_SERIAL_BAUD);
  ssdLog("Startup");
  pinMode(SSD_LED_PIN, OUTPUT);
  pinMode(SSD_BELL_RELAY_PIN, OUTPUT);
  pinMode(SSD_BUTTON_OUT_PIN, OUTPUT);
  pinMode(SSD_BUTTON_IN_PIN, INPUT);
  digitalWrite(SSD_BELL_RELAY_PIN, LOW);
  digitalWrite(SSD_BUTTON_OUT_PIN, LOW); // could also just be connected to GND
  ssdLog("Connecting to WiFi SSID: " SSD_SSID);;
  WiFi.mode(WIFI_STA);
  WiFi.begin(SSD_SSID, SSD_PSK);
  ssdLog("MQTT server: "SSD_MQTT_SERVER);
  mqttClient.setServer(SSD_MQTT_SERVER, SSD_MQTT_PORT);
  mqttClient.setSocketTimeout(SSD_MQTT_TIMEOUT);
  mqttClient.setBufferSize(SSD_MQTT_BUFFER_SIZE);
  ssdLog("Startup finished");
}

void loop() {
  // put your main code here, to run repeatedly:
  ssdLog("Starting loop");
  int ssdButtonState = digitalRead(SSD_BUTTON_IN_PIN);
  while (true) {
    ssdButtonState = digitalRead(SSD_BUTTON_IN_PIN);
    if (ssdButtonState == LOW) {
      if (ssdButtonPressed == 0) {
        ssdButtonPressed = millis();
      }
      if (ssdButtonPressedFor() > SSD_BUTTON_TIME) {
        digitalWrite(SSD_BELL_RELAY_PIN, HIGH);
        ssdLog("TRIGGER!");
        if (ssdLastTriggered() > SSD_MQTT_ALERT_BACKOFF) {
          ssdSendMqtt(ssdMqttActionTriggerTopic, "dingdong");
          ssdTriggered = millis();
        }
      }
    }
    else {
      digitalWrite(SSD_BELL_RELAY_PIN, LOW);
      ssdButtonPressed = 0;
    }
    ssdFlashLeds();
    ssdMqttReconnect();
    mqttClient.loop();
    yield();
  }
}

long ssdButtonPressedFor() { return millis() - ssdButtonPressed; }
long ssdLastTriggered() { return millis() - ssdTriggered; }

void ssdSendMqtt(String topic, String message) {
  if (mqttClient.connected()) {
    mqttClient.publish(topic.c_str(), message.c_str());
    ssdLog("MQTT send topic: " + topic + " message: " + message);
  }
  else {
   ssdLog("MQTT is not connected");
  }
}

void ssdMqttReconnect() {
  if ((ssdLastMqttConnectAttempt() > SSD_MQTT_RETRY) || (ssdMqttFirstConnectAttempt == true)) {
    ssdMqttConnect = millis();
    if (WiFi.status() == WL_CONNECTED) {
      if (!mqttClient.connected()) {
        ssdMqttFirstConnectAttempt = false;
        ssdLog("MQTT connecting");
        if (mqttClient.connect(ssdId.c_str(), SSD_MQTT_USER, SSD_MQTT_PASS)) {
            ssdLog("MQTT connected");
            ssdSendMqtt(ssdMqttDiscoveryTriggerTopic, ssdMqttDiscoveryTriggerPayload);
            ssdSendMqtt(ssdMqttDiscoveryHeartbeatTopic, ssdMqttDiscoveryHeartbeatPayload);
            ssdSendMqtt(ssdMqttActionTriggerTopic, "hello");
        }
        else {
          ssdLog("MQTT connect unsuccessful");
        }
      }
      else {
        ssdSendMqtt(ssdMqttHeartbeatTopic, "OK");
      }
    }
    else if (!ssdMqttFirstConnectAttempt) {
      ssdLog("SSD WiFi not connected");
    }
  }
}

long ssdLastMqttConnectAttempt() { return millis() - ssdMqttConnect; }

void ssdFlashLeds() {
  int ssdLedOnPeriod;
  int ssdLedOffPeriod;
  if (WiFi.status() != WL_CONNECTED) {
    ssdLedOnPeriod = SSD_LED_ON_3;
    ssdLedOffPeriod = SSD_LED_OFF_3;
  }
  else if (!mqttClient.connected()) {
    ssdLedOnPeriod = SSD_LED_ON_4;
    ssdLedOffPeriod =  SSD_LED_OFF_4;
  }
  else {
    // all connected!
    ssdLedOnPeriod = SSD_LED_ON_1;
    ssdLedOffPeriod = SSD_LED_OFF_1;
  }

  if (ssdLedOn > 0) {
    if (ssdLedOnFor() < ssdLedOnPeriod) {
      digitalWrite(SSD_LED_PIN, LOW);
    }
    else {
      ssdLedOn = 0;
      ssdLedOff = millis();
    }
  }
  if (ssdLedOff > 0) {
    if (ssdLedOffFor() < ssdLedOffPeriod) {
      digitalWrite(SSD_LED_PIN, HIGH);
    }
    else {
      ssdLedOff = 0;
      ssdLedOn = millis();
    }
  }
}

long ssdLedOnFor() { return millis() - ssdLedOn; }
long ssdLedOffFor() { return millis() - ssdLedOff; }

void ssdLog(String message) {
  Serial.println("SSD> " + message);
}
