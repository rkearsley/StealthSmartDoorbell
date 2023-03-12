#define SSD_MQTT_PORT 1883
#define SSD_MQTT_AUTO_DISCOVERY "homeassistant"
#define SSD_MQTT_TIMEOUT 2
#define SSD_MQTT_RETRY 60000
#define SSD_MQTT_BUFFER_SIZE 1024
#define SSD_MQTT_ALERT_BACKOFF 5000

#define SSD_MODEL "Stealth Smart Doorbell / Feather HUZZAH ESP8266"
#define SSD_MANUFACTURER "rkearsley / Adafruit"
#define SSD_LED_PIN 0
#define SSD_BELL_RELAY_PIN 15
#define SSD_BUTTON_OUT_PIN 16
#define SSD_BUTTON_IN_PIN 2

#define SSD_BUTTON_TIME 10
#define SSD_LED_OFF_1 200
#define SSD_LED_ON_1 1800
#define SSD_LED_OFF_3 800
#define SSD_LED_ON_3 200
#define SSD_LED_OFF_4 500
#define SSD_LED_ON_4 500

#define SSD_SERIAL_BAUD 115200

static String ssdId = "StealthSmartDoorbell_" + String(ESP.getChipId());
static String ssdName = "Stealth Smart Doorbell";

static String ssdMqttActionTriggerTopic = ssdId + "/trigger";
static String ssdMqttDiscoveryTriggerTopic = SSD_MQTT_AUTO_DISCOVERY "/device_automation/" + ssdId + "/trigger/config";
static String ssdMqttDiscoveryTriggerPayload = 
  "{"
    "\"automation_type\":\"trigger\","
    "\"topic\":\"" + ssdMqttActionTriggerTopic + "\","
    "\"type\":\"button_short_press\","
    "\"subtype\":\"button_1\","
    "\"payload\":\"dingdong\","
    "\"device\":{"
      "\"identifiers\":[\"" + ssdId + "\"],"
      "\"name\":\"" + ssdName + "\","
      "\"model\":\"" SSD_MODEL "\","
      "\"manufacturer\":\"" SSD_MANUFACTURER "\","
      "\"connections\":[[\"mac\",\"" + WiFi.macAddress() + "\"]]"
    "}"
  "}";
      
static String ssdMqttHeartbeatTopic = ssdId + "/heartbeat";
static String ssdMqttDiscoveryHeartbeatTopic = SSD_MQTT_AUTO_DISCOVERY "/sensor/" + ssdId + "/heartbeat/config";
static String ssdMqttDiscoveryHeartbeatPayload = 
  "{"
    "\"state_topic\":\"" + ssdMqttHeartbeatTopic + "\","
    "\"name\":\"" + ssdName + " Heartbeat\","
    "\"unique_id\":\"" + ssdId + "_ok\","
    "\"expire_after\":120,"
    "\"device\":{"
      "\"identifiers\":[\"" + ssdId + "\"],"
      "\"name\":\"" + ssdName + "\","
      "\"model\":\"" SSD_MODEL "\","
      "\"manufacturer\":\"" SSD_MANUFACTURER "\","
      "\"connections\":[[\"mac\",\"" + WiFi.macAddress() + "\"]]"
    "}"
  "}";
