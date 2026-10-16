#ifndef ULTRASONIC_H
#define ULTRASONIC_H

#include <Arduino.h>

class SensorUltrasonico
{
private:
    uint8_t trigPin;
    uint8_t echoPin;

    static const uint8_t BUFFER_SIZE = 5;

    float buffer[BUFFER_SIZE];
    uint8_t indiceBuffer;
    uint8_t muestrasValidas;

    float ultimaDistancia;
    unsigned long ultimaMedicion;

    void disparar();
    float leerPulsoCm();
    float calcularMediana();

public:
    SensorUltrasonico(uint8_t trig, uint8_t echo);
    void begin();
    void update(unsigned long ahora);
    float getDistanceCm();
};

#endif