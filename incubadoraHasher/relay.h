#pragma once
#include <Arduino.h>

void relayInit();
void relayLoop();

void relaySet(bool on);
bool relayIsOn();