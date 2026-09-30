#ifndef sensoresUltrasonicos_h
#define sensoresUltrasonicos_h

#include <Arduino.h>

void iniciarUltrasonicos();
float leerUltrasonico(int trig, int echo);

#endif