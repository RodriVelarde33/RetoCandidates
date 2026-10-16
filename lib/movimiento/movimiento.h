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
    GIRAR,
    CENTRAR
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
    unsigned long tGiroEstable;
    bool giroEstable;

    bool atasco;
    float ultimaDistanciaFrontal;
    unsigned long tUltimoProgreso;

    void aplicarMotores(int vIzq, int vDer);
    void girarConError(float error, int pwmBase);

public:
    Movimiento(PuenteH &puente, SensorUltrasonico &frontal,
               SensorUltrasonico &izquierdo, SensorUltrasonico &derecho,
               SensorColor &colorFrontal, Garra &garra, SensorGiro &giro);

    bool begin();
    void update();

    void mover(int vIzqReal, int vDerReal);
    void avanzar();
    void retroceder();
    void detener();

    float getDistanciaFrontal();
    float getDistanciaIzquierda();
    float getDistanciaDerecha();

    void actualizarRumbo();
    void fijarRumbo();
    float getRumbo() const { return rumbo; }

    void startForward();
    void startBackwardMs(unsigned long ms);
    void startTurn(float grados);
    void startCentering();

    bool isManeuverDone() const;
    bool isStuck() const { return atasco; }

    void avanzarConParedes(float dIzq, float dDer);

    void girarGrados(float grados);
    void girarDerecha90();
    void girarIzquierda90();
    void girar180();

    void retrocederTiempo(unsigned long ms);

    void setVelocidad(int v) { velocidad = v; }
    void setVelocidadGiro(int v) { velocidadGiro = v; }
};

#endif