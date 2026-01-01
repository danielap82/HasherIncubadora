#pragma once
#include <Arduino.h>

void wifiInit();
bool wifiIsAPMode();
String wifiGetIPString();