#ifndef MOVIMIENTO_H
#define MOVIMIENTO_H

#include <Arduino.h>

// Rutas relativas respetando exactamente las mayúsculas de tus carpetas en lib/
#include "../moduloPuenteH/moduloPuenteH.h"
#include "../sensorUltrasonico/sensorUltrasonico.h"
#include "../SensorDeColor/sensorColor.h"
#include "../Garra/garra.h"

#ifndef DISTANCIA_MINIMA
#define DISTANCIA_MINIMA 20 // Distancia en centímetros
#endif

class Movimiento
{
private:
    PuenteH &puente;
    SensorUltrasonico &frontal;
    SensorColor &colorFrontal;
    Garra &garra;
    int velocidad;

public:
    Movimiento(PuenteH &puente, SensorUltrasonico &frontal, SensorColor &colorFrontal, Garra &garra);

    void begin();
    void avanzar();
    void retroceder();
    void detener();
    void actualizar();
};

#endif