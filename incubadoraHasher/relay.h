#ifndef RELAY_H
#define RELAY_H

void relayInit();
void relaySet(bool on);
bool relayIsOn();
void relayLoop();
unsigned long relayGetSecondsSinceActivation();

extern unsigned long ultimoRiego;
extern float humedadEnUltimoRiego;
extern float temperaturaEnUltimoRiego;

#endif