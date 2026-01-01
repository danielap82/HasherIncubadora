#include <Wire.h>
#include "Adafruit_SHT4x.h"
#include "sensor.h"

Adafruit_SHT4x sht4;

// Variables de lectura actual
float h = 0.0f, t = 0.0f;
bool sensorOK = false;

// Historial
float historyTemp[HISTORY_SIZE];
float historyHum[HISTORY_SIZE];
unsigned int historyIndex = 0;
bool historyFull = false;

// Intervalo entre muestras
static const unsigned long SAMPLE_INTERVAL = 5000; // 5 s
static unsigned long lastSampleMillis = 0;

// Watchdog del sensor
unsigned long lastValidSensorMillis = 0;
const unsigned long SENSOR_TIMEOUT = 180000; // 3 minutos

// ----------------------------------------------------
// Añadir muestra al historial
// ----------------------------------------------------
void addSample(float temp, float hum) {
  historyTemp[historyIndex] = temp;
  historyHum[historyIndex]  = hum;

  historyIndex++;
  if (historyIndex >= HISTORY_SIZE) {
    historyIndex = 0;
    historyFull = true;
  }
}

// ----------------------------------------------------
// Inicialización del sensor
// ----------------------------------------------------
void sensorInit() {
  // Inicializar historial
  for (unsigned int i = 0; i < HISTORY_SIZE; i++) {
    historyTemp[i] = NAN;
    historyHum[i] = NAN;
  }

  if (!sht4.begin()) {
    Serial.println("ERROR: No se detectó el sensor SHT4x");
    sensorOK = false;
  } else {
    sensorOK = true;
    sht4.setPrecision(SHT4X_HIGH_PRECISION);
    sht4.setHeater(SHT4X_NO_HEATER);
    lastValidSensorMillis = millis();
  }
}

// ----------------------------------------------------
// Lectura del sensor con validación
// ----------------------------------------------------
void leerSensor() {
  sensors_event_t humidity, temp;
  sht4.getEvent(&humidity, &temp);

  float newH = humidity.relative_humidity;
  float newT = temp.temperature;

  bool lecturaValida = true;

  // Validaciones
  if (isnan(newH) || isnan(newT)) lecturaValida = false;
  if (newH <= 0 || newT <= 0) lecturaValida = false;
  if (newH > 100 || newT > 80) lecturaValida = false;

  if (lecturaValida) {
    h = newT;
    t = newH;
    lastValidSensorMillis = millis();
  } else {
    Serial.println("⚠ Lectura inválida del sensor SHT4x");
  }
}

// ----------------------------------------------------
// Bucle del sensor
// ----------------------------------------------------
void sensorLoop() {
  if (!sensorOK) return;

  leerSensor();

  unsigned long ahora = millis();

  // Guardar en historial
  if (ahora - lastSampleMillis >= SAMPLE_INTERVAL) {
    addSample(t, h);
    lastSampleMillis = ahora;
  }

  // Watchdog: reiniciar si falla 3 minutos
  if (millis() - lastValidSensorMillis > SENSOR_TIMEOUT) {
    Serial.println("❌ Sensor sin datos válidos durante 3 minutos. Reiniciando...");
    delay(500);
    ESP.restart();
  }
}

// ----------------------------------------------------
// Getters
// ----------------------------------------------------
float sensorGetTemp() { return t; }
float sensorGetHum()  { return h; }
bool sensorIsOK()     { return sensorOK; }

unsigned int sensorGetHistoryCount() {
  return historyFull ? HISTORY_SIZE : historyIndex;
}

unsigned long sensorGetSampleIntervalSeconds() {
  return SAMPLE_INTERVAL / 1000;
}