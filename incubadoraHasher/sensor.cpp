#include "sensor.h"
#include <Adafruit_SHT4x.h>
#include "config.h"
#include <Arduino.h>

Adafruit_SHT4x sht4;
static bool sensor_ok = false;

// Últimas lecturas válidas
static float ultimaHumValida = 50.0;
static float ultimaTempValida = 25.0;

// -------------------------
// HISTORIAL ANTIGUO
// -------------------------
float historyTemp[HISTORY_SIZE];
float historyHum[HISTORY_SIZE];
static unsigned int historyCount = 0;
static unsigned long lastSampleTime = 0;
static const unsigned long SAMPLE_INTERVAL = 60000; // 1 minuto

// -------------------------

bool sensorInit() {
    if (!sht4.begin()) {
        sensor_ok = false;
        return false;
    }
    sht4.setPrecision(SHT4X_HIGH_PRECISION);
    sht4.setHeater(SHT4X_NO_HEATER);
    sensor_ok = true;
    return true;
}

bool sensorIsOK() {
    return sensor_ok;
}

float sensorGetHumRaw() {
    if (!sensor_ok) return ultimaHumValida;

    sensors_event_t hum, temp;
    if (!sht4.getEvent(&hum, &temp)) {
        return ultimaHumValida;
    }
    return hum.relative_humidity;
}

float sensorGetTempRaw() {
    if (!sensor_ok) return ultimaTempValida;

    sensors_event_t hum, temp;
    if (!sht4.getEvent(&hum, &temp)) {
        return ultimaTempValida;
    }
    return temp.temperature;
}

float sensorGetHum() {
    float h = sensorGetHumRaw();

    if (isnan(h) || h < 5 || h > 100) {
        return ultimaHumValida;
    }

    if (fabs(h - ultimaHumValida) > 20) {
        return ultimaHumValida;
    }

    ultimaHumValida = h;
    return h;
}

float sensorGetTemp() {
    float t = sensorGetTempRaw();

    if (isnan(t) || t < -20 || t > 80) {
        return ultimaTempValida;
    }

    if (fabs(t - ultimaTempValida) > 15) {
        return ultimaTempValida;
    }

    ultimaTempValida = t;
    return t;
}

// -------------------------
// HISTORIAL ANTIGUO
// -------------------------
void sensorLoop() {
    unsigned long now = millis();

    if (now - lastSampleTime < SAMPLE_INTERVAL)
        return;

    lastSampleTime = now;

    float h = sensorGetHum();
    float t = sensorGetTemp();

    if (historyCount < HISTORY_SIZE) {
        historyHum[historyCount] = h;
        historyTemp[historyCount] = t;
        historyCount++;
    } else {
        // Desplazar todo 1 posición hacia arriba
        for (int i = 1; i < HISTORY_SIZE; i++) {
            historyHum[i - 1] = historyHum[i];
            historyTemp[i - 1] = historyTemp[i];
        }
        historyHum[HISTORY_SIZE - 1] = h;
        historyTemp[HISTORY_SIZE - 1] = t;
    }
}

unsigned int sensorGetHistoryCount() {
    return historyCount;
}

unsigned long sensorGetSampleIntervalSeconds() {
    return SAMPLE_INTERVAL / 1000;
}