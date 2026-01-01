#pragma once
#include <Arduino.h>

// Tamaño del historial
const unsigned int HISTORY_SIZE = 1200;

// Variables globales del historial (declaración)
extern float historyTemp[HISTORY_SIZE];
extern float historyHum[HISTORY_SIZE];
extern unsigned int historyIndex;
extern bool historyFull;

// Inicialización y ciclo del sensor
void sensorInit();
void sensorLoop();

// Lecturas actuales
float sensorGetTemp();
float sensorGetHum();
bool sensorIsOK();

// Historial
unsigned int sensorGetHistoryCount();
unsigned long sensorGetSampleIntervalSeconds();

