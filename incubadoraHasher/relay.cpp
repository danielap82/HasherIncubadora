#include "relay.h"
#include "sensor.h"
#include "config.h"
#include <Arduino.h>

const int relePin = 12; // D6 NodeMCU

static bool releActivo = false;
unsigned long ultimoRiego = 0;
static unsigned long inicioRiego = 0;
unsigned long relayLastActivation = 0;

float humedadEnUltimoRiego = 0;
float temperaturaEnUltimoRiego = 0;
float ultimaHumRiego = 0;
float ultimaTempRiego = 0;
unsigned long ultimoRiegoTimestamp = 0;

const unsigned long RELAY_MAX_ON_TIME = 300000; // 5 min
static int lecturasBajas = 0;

void relayInit() {
    pinMode(relePin, OUTPUT);
    digitalWrite(relePin, HIGH); // reposo
    releActivo = false;
    ultimoRiego = 0;
    inicioRiego = 0;
    relayLastActivation = millis();
    lecturasBajas = 0;
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
    unsigned long ahora = millis();

    if (releActivo && inicioRiego > 0) {
        if (ahora - inicioRiego > RELAY_MAX_ON_TIME) {
            Serial.println("Protección anti-bloqueo: relé demasiado tiempo ON. Apagando...");
            relaySet(false);
            inicioRiego = 0;
        }
    }

    if (releActivo) {
        if (ahora - inicioRiego >= cfg.tiempoRiego) {
            relaySet(false);
            ultimoRiego = ahora;
            inicioRiego = 0;
            Serial.println("Riego finalizado.");
        }
        return;
    }

    if (!sensorIsOK()) {
        lecturasBajas = 0;
        return;
    }

    float humedad = sensorGetHum();

    if (ahora - ultimoRiego < cfg.tiempoEspera) {
        lecturasBajas = 0;
        return;
    }

    if (humedad < cfg.umbralHumedad) {
        lecturasBajas++;
    } else {
        lecturasBajas = 0;
    }

    if (lecturasBajas < 3) return;

    humedadEnUltimoRiego = humedad;
    temperaturaEnUltimoRiego = sensorGetTemp();
    ultimaHumRiego = sensorGetHum();
    ultimaTempRiego = sensorGetTemp();
    ultimoRiegoTimestamp = millis();
    Serial.println("Humedad baja durante 3 lecturas. Iniciando riego...");
    relaySet(true);
    inicioRiego = ahora;
    lecturasBajas = 0;
}

unsigned long relayGetSecondsSinceActivation() {
    if (relayLastActivation == 0) return 0;
    return (millis() - relayLastActivation) / 1000;
}