#include "relay.h"
#include "sensor.h"
#include "config.h"

const int relePin = 12; // D6 NodeMCU
static bool releActivo = false;
static unsigned long ultimoRiego = 0;

void relayInit() {
  pinMode(relePin, OUTPUT);
  // Relé activo en LOW → reposo HIGH
  digitalWrite(relePin, HIGH);
  releActivo = false;
}

void relaySet(bool on) {
  releActivo = on;
  if (on) {
    digitalWrite(relePin, LOW);
  } else {
    digitalWrite(relePin, HIGH);
  }
}

bool relayIsOn() {
  return releActivo;
}

void relayLoop() {
  // Control automático solo si el sensor está OK
  if (!sensorIsOK()) return;

  unsigned long ahora = millis();

  if (ahora - ultimoRiego >= cfg.tiempoEspera) {
    if (sensorGetHum() < cfg.umbralHumedad) {
      Serial.println("Humedad por debajo del umbral. Activando actuador...");
      relaySet(true);
      delay(cfg.tiempoRiego);
      relaySet(false);
      ultimoRiego = millis();
    }
  }
}