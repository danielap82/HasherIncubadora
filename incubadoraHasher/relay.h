#pragma once
#include <Arduino.h>

void relayInit();
void relayLoop();

void relaySet(bool on);
bool relayIsOn();

// Tiempo desde la última activación (s)
unsigned long relayGetSecondsSinceActivation();