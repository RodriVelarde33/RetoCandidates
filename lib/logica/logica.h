#ifndef LOGICA_H
#define LOGICA_H

#include <Arduino.h>
#include "../movimiento/movimiento.h"
#include "../SensorDeColor/sensorColor.h"
#include "../Garra/garra.h"

void pistaA_init(Movimiento &mov);
bool pistaA_ejecutar(Movimiento &mov, SensorColor &color);

void mostrarColor(ColorDetectado color);
void apagarLEDs();

#endif