#include <Arduino.h>
#include "config.h"
#include "wifi_net.h"
#include "sensor.h"
#include "relay.h"
#include "mqtt_mod.h"
#include "web.h"

void setup() {
  Serial.begin(115200);
  delay(200);

  // Inicializar configuración (EEPROM, valores por defecto)
  configInit();

  // Inicializar hardware
  relayInit();     // Relé primero (para dejarlo en reposo)
  sensorInit();    // Sensor SHT4x
  wifiInit();      // WiFi STA/AP según config
 // mqttInit();      // Configura MQTT (a partir de cfg)
  webInit();       // Webserver, OTA, API, manual

  Serial.println("Setup completo. Incubadora Hasher lista.");
}

void loop() {
  // Webserver (HTTP + OTA)
  webLoop();

  // WiFi se gestiona por la propia librería
  // Sensor + watchdog + historial
  sensorLoop();

  // Lógica de control del relé (automático)
  relayLoop();

  // MQTT (reconexión, publicación, recepción)
 // mqttLoop();
}