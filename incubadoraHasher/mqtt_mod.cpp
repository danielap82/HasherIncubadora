#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "config.h"
#include "wifi_net.h"
#include "sensor.h"
#include "relay.h"

WiFiClient espClient;
PubSubClient mqttClient(espClient);

const char* MQTT_CLIENT_ID_BASE = "incubadora_hasher_esp8266";
const char* MQTT_BASE_TOPIC     = "incubadora_hasher";

unsigned long lastMqttPublish = 0;
const unsigned long MQTT_PUBLISH_INTERVAL = 5000;

void publishMqttState();
void publishMqttDiscovery();

void mqttCallback(char* topic, byte* payload, unsigned int length) {
  String tpc = String(topic);
  String msg;
  for (unsigned int i = 0; i < length; i++) {
    msg += (char)payload[i];
  }
  msg.trim();

  String base = String(MQTT_BASE_TOPIC);

  if (tpc == base + "/control/umbral") {
    float nuevoUmbral = msg.toFloat();
    if (nuevoUmbral > 0 && nuevoUmbral < 100) {
      cfg.umbralHumedad = nuevoUmbral;
      configSave();
    }
  } else if (tpc == base + "/control/intervalo") {
    unsigned long nuevoInt = msg.toInt();
    if (nuevoInt >= 5000 && nuevoInt <= 3600000) {
      cfg.tiempoEspera = nuevoInt;
      configSave();
    }
  } else if (tpc == base + "/control/tiempo") {
    unsigned long nuevoTiempo = msg.toInt();
    if (nuevoTiempo >= 100 && nuevoTiempo <= 600000) {
      cfg.tiempoRiego = nuevoTiempo;
      configSave();
    }
  } else if (tpc == base + "/control/rele") {
    if (msg.equalsIgnoreCase("ON") || msg == "1") {
      relaySet(true);
    } else if (msg.equalsIgnoreCase("OFF") || msg == "0") {
      relaySet(false);
    }
  }
}

void mqttReconnect() {
  if (strlen(cfg.mqttHost) == 0) return;
  if (wifiIsAPMode()) return;
  if (WiFi.status() != WL_CONNECTED) return;
  if (mqttClient.connected()) return;

  Serial.print("Conectando a MQTT en ");
  Serial.print(cfg.mqttHost);
  Serial.print(":");
  Serial.println(cfg.mqttPort);

  String clientId = String(MQTT_CLIENT_ID_BASE) + "_" + String(ESP.getChipId(), HEX);

  bool connected;
  if (strlen(cfg.mqttUser) > 0) {
    connected = mqttClient.connect(clientId.c_str(), cfg.mqttUser, cfg.mqttPass);
  } else {
    connected = mqttClient.connect(clientId.c_str());
  }

  if (connected) {
    Serial.println("MQTT conectado");
    String base = String(MQTT_BASE_TOPIC);
    mqttClient.subscribe((base + "/control/umbral").c_str());
    mqttClient.subscribe((base + "/control/intervalo").c_str());
    mqttClient.subscribe((base + "/control/tiempo").c_str());
    mqttClient.subscribe((base + "/control/rele").c_str());

    publishMqttDiscovery();
  } else {
    Serial.print("Fallo MQTT, rc=");
    Serial.println(mqttClient.state());
  }
}

void mqttInit() {
  if (strlen(cfg.mqttHost) > 0) {
    mqttClient.setServer(cfg.mqttHost, cfg.mqttPort > 0 ? cfg.mqttPort : 1883);
    mqttClient.setCallback(mqttCallback);
  }
}

void mqttLoop() {
  if (strlen(cfg.mqttHost) == 0) return;
  if (wifiIsAPMode()) return;

  if (!mqttClient.connected()) {
    mqttReconnect();
  }
  mqttClient.loop();

  unsigned long ahora = millis();
  if (mqttClient.connected() && ahora - lastMqttPublish >= MQTT_PUBLISH_INTERVAL) {
    publishMqttState();
    lastMqttPublish = ahora;
  }
}

void publishMqttState() {
  String base = String(MQTT_BASE_TOPIC);

  String tempTopic = base + "/estado/temp";
  String humTopic  = base + "/estado/hum";
  String releTopic = base + "/estado/rele";
  String modeTopic = base + "/estado/mode";

  mqttClient.publish(tempTopic.c_str(), String(sensorGetTemp(), 2).c_str(), true);
  mqttClient.publish(humTopic.c_str(),  String(sensorGetHum(),  2).c_str(), true);
  mqttClient.publish(releTopic.c_str(), relayIsOn() ? "ON" : "OFF", true);
  mqttClient.publish(modeTopic.c_str(), wifiIsAPMode() ? "AP" : "STA", true);
}

void publishMqttDiscovery() {
  String nodeId     = "incubadora_hasher";
  String deviceName = "Incubadora Hasher";
  String ipStr      = wifiGetIPString();

  // Temp
  {
    String confTopic = "homeassistant/sensor/" + nodeId + "_temp/config";
    String stateTopic = String(MQTT_BASE_TOPIC) + "/estado/temp";

    String payload = "{";
    payload += "\"name\":\"Incubadora Temp\",";
    payload += "\"state_topic\":\"" + stateTopic + "\",";
    payload += "\"unit_of_measurement\":\"°C\",";
    payload += "\"device_class\":\"temperature\",";
    payload += "\"unique_id\":\"" + nodeId + "_temp\",";
    payload += "\"device\":{";
    payload += "\"identifiers\":[\"" + nodeId + "\"],";
    payload += "\"name\":\"" + deviceName + "\",";
    payload += "\"model\":\"Incubadora Hasher\",";
    payload += "\"manufacturer\":\"Daniel\",";
    payload += "\"configuration_url\":\"http://" + ipStr + "\"";
    payload += "}";
    payload += "}";

    mqttClient.publish(confTopic.c_str(), payload.c_str(), true);
  }

  // Hum
  {
    String confTopic = "homeassistant/sensor/" + nodeId + "_hum/config";
    String stateTopic = String(MQTT_BASE_TOPIC) + "/estado/hum";

    String payload = "{";
    payload += "\"name\":\"Incubadora Hum\",";
    payload += "\"state_topic\":\"" + stateTopic + "\",";
    payload += "\"unit_of_measurement\":\"%\",";
    payload += "\"device_class\":\"humidity\",";
    payload += "\"unique_id\":\"" + nodeId + "_hum\",";
    payload += "\"device\":{";
    payload += "\"identifiers\":[\"" + nodeId + "\"],";
    payload += "\"name\":\"" + deviceName + "\",";
    payload += "\"model\":\"Incubadora Hasher\",";
    payload += "\"manufacturer\":\"Daniel\",";
    payload += "\"configuration_url\":\"http://" + ipStr + "\"";
    payload += "}";
    payload += "}";

    mqttClient.publish(confTopic.c_str(), payload.c_str(), true);
  }
}