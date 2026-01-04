#include "relay.h"
#include "sensor.h"
#include "config.h"

const int relePin = 12; // D6 NodeMCU

static bool releActivo = false;
static unsigned long ultimoRiego = 0;

// Registro de última activación
unsigned long relayLastActivation = 0;

// Protección anti-bloqueo: tiempo máximo que el relé puede estar ON seguido
const unsigned long RELAY_MAX_ON_TIME = 300000; // 5 minutos

void relayInit() {
  pinMode(relePin, OUTPUT);
  digitalWrite(relePin, HIGH); // Relé activo en LOW → reposo HIGH
  releActivo = false;
  relayLastActivation = 0;
}

void relaySet(bool on) {
  releActivo = on;
  if (on) {
    digitalWrite(relePin, LOW);
    relayLastActivation = millis();
  } else {
    digitalWrite(relePin, HIGH);
  }
}

bool relayIsOn() {
  return releActivo;
}

void relayLoop() {
  // Protección anti-bloqueo: si por lo que sea se queda encendido demasiado tiempo, lo apagamos
  if (releActivo && relayLastActivation > 0) {
    if (millis() - relayLastActivation > RELAY_MAX_ON_TIME) {
      Serial.println("Protección anti-bloqueo: relé llevaba demasiado tiempo activado. Apagando...");
      relaySet(false);
    }
  }

  // Control automático solo si el sensor está OK
  if (!sensorIsOK()) return;

  unsigned long ahora = millis();

  if (ahora - ultimoRiego >= cfg.tiempoEspera) {
    if (sensorGetHum() < cfg.umbralHumedad) {
      Serial.println("Humedad por debajo del umbral. Activando actuador (modo automático)...");
      relaySet(true);
      delay(cfg.tiempoRiego);
      relaySet(false);
      ultimoRiego = millis();
    }
  }
}

// Segundos desde la última activación
unsigned long relayGetSecondsSinceActivation() {
  if (relayLastActivation == 0) return 0;
  return (millis() - relayLastActivation) / 1000;
}