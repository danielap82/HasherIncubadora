#include <Wire.h>
#include <Adafruit_SHT31.h>
#include "sensor.h"

Adafruit_SHT31 sht31 = Adafruit_SHT31();

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
// Inicialización del bus I2C según la placa
// ----------------------------------------------------
void initI2C() {
#if defined(ESP8266)
    // NodeMCU / Wemos D1 Mini
    Wire.begin(D2, D1);   // SDA = D2 (GPIO4), SCL = D1 (GPIO5)
    Serial.println("I2C inicializado para ESP8266 (NodeMCU)");
#elif defined(ESP32)
    Wire.begin(21, 22);   // Pines estándar ESP32
    Serial.println("I2C inicializado para ESP32");
#else
    Wire.begin();         // Arduino u otras placas
    Serial.println("I2C inicializado (modo genérico)");
#endif
}

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
  initI2C();

  // Inicializar historial
  for (unsigned int i = 0; i < HISTORY_SIZE; i++) {
    historyTemp[i] = NAN;
    historyHum[i] = NAN;
  }

  if (!sht31.begin(0x44)) {
    Serial.println("ERROR: No se detectó el sensor SHT30/SHT31");
    sensorOK = false;
  } else {
    sensorOK = true;
    lastValidSensorMillis = millis();
    Serial.println("SHT30 inicializado correctamente");
  }
}

// ----------------------------------------------------
// Lectura del sensor con validación
// ----------------------------------------------------
void leerSensor() {
  float newT = sht31.readTemperature();
  float newH = sht31.readHumidity();

  bool lecturaValida = true;

  if (isnan(newT) || isnan(newH)) lecturaValida = false;
  if (newT < 5 || newT > 80) lecturaValida = false;
  if (newH < 0 || newH > 100) lecturaValida = false;

  if (lecturaValida) {
    t = newT;
    h = newH;
    lastValidSensorMillis = millis();
  } else {
    Serial.println("⚠ Lectura inválida del sensor SHT30");
  }
}

// ----------------------------------------------------
// Bucle del sensor
// ----------------------------------------------------
void sensorLoop() {
  if (!sensorOK) return;

  leerSensor();

  unsigned long ahora = millis();

  if (ahora - lastSampleMillis >= SAMPLE_INTERVAL) {
    addSample(t, h);
    lastSampleMillis = ahora;
  }

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