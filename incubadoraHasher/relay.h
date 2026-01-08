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
extern float ultimaHumRiego;
extern float ultimaTempRiego;
extern unsigned long ultimoRiegoTimestamp;
#endif