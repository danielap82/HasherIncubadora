#ifndef SENSOR_H
#define SENSOR_H

bool sensorInit();
bool sensorIsOK();
float sensorGetHum();
float sensorGetTemp();

// Historial antiguo
void sensorLoop();
unsigned int sensorGetHistoryCount();
unsigned long sensorGetSampleIntervalSeconds();

extern float historyTemp[];
extern float historyHum[];

#endif