#ifndef MOVIMIENTO_H
#define MOVIMIENTO_H

#include <Arduino.h>
#include "../moduloPuenteH/moduloPuenteH.h"
#include "../sensorUltrasonico/sensorUltrasonico.h"
#include "../SensorDeColor/sensorColor.h"
#include "../sensorGiro/sensorGiro.h"
#include "../Garra/garra.h"

enum class Maniobra
{
    NINGUNA,
    AVANZAR,
    RETROCEDER_TIEMPO,
    GIRAR
};

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

    Maniobra maniobra;
    unsigned long tManiobra;
    unsigned long finRetroceso;

    float objetivoGiro;
    bool giroEstable;

    bool atasco;
    float ultimaDistanciaFrontal;
    unsigned long tUltimoProgreso;

    void aplicarMotores(int vIzq, int vDer);
    void girarConError(float error, int pwmBase);

public:
    Movimiento(PuenteH &p, SensorUltrasonico &f, SensorUltrasonico &i, SensorUltrasonico &d,
               SensorColor &c, Garra &g, SensorGiro &gi);

    bool begin();
    void update();
    void detener();
    void actualizarRumbo();

    float getDistanciaFrontal();
    float getDistanciaIzquierda();
    float getDistanciaDerecha();

    void startForward();
    void startBackwardMs(unsigned long ms);
    void startTurn(float grados);

    bool isManeuverDone() const;
    bool isStuck() const { return atasco; }

    void setVelocidad(int v) { velocidad = v; }
    void setVelocidadGiro(int v) { velocidadGiro = v; }
};

#endif