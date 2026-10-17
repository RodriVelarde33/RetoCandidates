#ifndef LOGICA_H
#define LOGICA_H

#include <Arduino.h>
#include "../movimiento/movimiento.h"
#include "../SensorDeColor/sensorColor.h"
#include "../Garra/garra.h"

// Pista A
void pistaA_init(Movimiento &mov);
bool pistaA_ejecutar(Movimiento &mov, SensorColor &color);

// Pista B
void pistaB_init(Movimiento &mov, Garra &garra, uint8_t seccionInicial);
bool pistaB_ejecutar(Movimiento &mov, SensorColor &colorFrontal, Garra &garra);

// Utilidades
void mostrarColor(ColorDetectado color);
void apagarLEDs();

#endif