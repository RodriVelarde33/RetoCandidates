#ifndef MOVIMIENTO_H
#define MOVIMIENTO_H
#include <Arduino.h>
#include <moduloPuenteH.h>
#include <sensorUltrasonico.h>

class Movimiento
{
private:
    PuenteH &puente;
    SensorUltrasonico &frontal;

    uint8_t velocidad;

    const float DISTANCIA_MINIMA = 15.0;

public:
    Movimiento(PuenteH &puente, SensorUltrasonico &frontal);

    void begin();

    void avanzar();
    void retroceder();
    void detener();

    void actualizar();
};

#endif