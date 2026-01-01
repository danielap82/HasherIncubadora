#include <EEPROM.h>
#include "config.h"

Config cfg;

void configSave() {
  EEPROM.put(EEPROM_ADDR, cfg);
  EEPROM.commit();
}

void configInit() {
  EEPROM.begin(1024);
  EEPROM.get(EEPROM_ADDR, cfg);

  // Si no hay SSID, asumimos EEPROM sin configurar
  if (strlen(cfg.ssid) < 1) {
    strcpy(cfg.ssid, "");
    strcpy(cfg.pass, "");

    strcpy(cfg.ip, "");
    strcpy(cfg.gateway, "");
    strcpy(cfg.subnet, "");
    strcpy(cfg.dns, "");

    cfg.umbralHumedad = 68.0;
    cfg.tiempoEspera  = 20000;  // 20 s
    cfg.tiempoRiego   = 1000;   // 1 s

    strcpy(cfg.mqttHost, "");
    cfg.mqttPort = 1883;
    strcpy(cfg.mqttUser, "");
    strcpy(cfg.mqttPass, "");

    configSave();
  }
}