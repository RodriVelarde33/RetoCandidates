#ifndef MOVIMIENTO_H
#define MOVIMIENTO_H

#include <Arduino.h>
#include "../moduloPuenteH/moduloPuenteH.h"
#include "../sensorUltrasonico/sensorUltrasonico.h"
#include "../SensorDeColor/sensorColor.h"
#include "../sensorGiro/sensorGiro.h"
#include "../Garra/garra.h"

class Movimiento
{
private:
    PuenteH &puente;
    SensorUltrasonico &frontal;
    SensorUltrasonico &izquierdo;
    SensorUltrasonico &derecho;
    SensorColor &colorFrontal;
    Garra &garra;
    SensorGiro &giro;
    int velocidad;
    int velocidadGiro;
    float offsetGiroZ;

    float rumbo;
    float rumboObjetivo;
    unsigned long tRumbo;

public:
    Movimiento(PuenteH &puente,
               SensorUltrasonico &frontal,
               SensorUltrasonico &izquierdo,
               SensorUltrasonico &derecho,
               SensorColor &colorFrontal,
               Garra &garra,
               SensorGiro &giro);

    bool begin();

    // Motores
    void mover(int vIzqReal, int vDerReal);
    void avanzar();
    void retroceder();
    void detener();

    // Ultrasonicos
    float getDistanciaFrontal();
    float getDistanciaIzquierda();
    float getDistanciaDerecha();

    // Giroscopio
    void actualizarRumbo();
    void fijarRumbo();
    float getRumbo() { return rumbo; }

    // Avance
    void avanzarConParedes(float dIzq, float dDer);

    // Giros
    void girarGrados(float grados);
    void girarDerecha90() { girarGrados(90); }
    void girarIzquierda90() { girarGrados(-90); }
    void girar180();

    // Retroceso
    void retrocederTiempo(unsigned long ms);

    void setVelocidad(int v) { velocidad = v; }
    void setVelocidadGiro(int v) { velocidadGiro = v; }
};

#endif