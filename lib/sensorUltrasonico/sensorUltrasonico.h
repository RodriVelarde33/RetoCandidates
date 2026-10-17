#ifndef ULTRASONIC_H
#define ULTRASONIC_H

#include <Arduino.h>

#define BUFFER_SIZE 3
#define ULTRASONICO_MIN_CM 2.0f
#define ULTRASONICO_MAX_CM 200.0f
#define ULTRASONICO_TIMEOUT_US 25000UL
#define ULTRASONICO_INTERVALO_MS 40UL

class SensorUltrasonico
{
private:
    uint8_t trigPin;
    uint8_t echoPin;

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