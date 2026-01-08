#pragma once
#include <Arduino.h>

// -----------------------------
// Tamaño del historial (global)
// -----------------------------
#define HISTORY_SIZE 60
// o si prefieres:
// const int HISTORY_SIZE = 60;
// -----------------------------

struct Config {
  char ssid[32];
  char pass[32];

  // IP fija opcional (si están vacíos => DHCP)
  char ip[16];
  char gateway[16];
  char subnet[16];
  char dns[16];

  // Parámetros de control
  float umbralHumedad;        // % humedad
  unsigned long tiempoEspera; // ms entre ciclos de control
  unsigned long tiempoRiego;  // ms de activación del relé

  // MQTT
  char mqttHost[32];
  uint16_t mqttPort;
  char mqttUser[32];
  char mqttPass[32];
};

// Config global
extern Config cfg;

// Dirección base EEPROM
#define EEPROM_ADDR 0

void configInit();
void configSave();