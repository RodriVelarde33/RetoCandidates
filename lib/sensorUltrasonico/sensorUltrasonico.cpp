#include "sensorUltrasonico.h"

SensorUltrasonico::SensorUltrasonico(uint8_t trig, uint8_t echo)
{
    trigPin = trig;
    echoPin = echo;
}

void SensorUltrasonico::begin()
{
    pinMode(trigPin, OUTPUT);
    pinMode(echoPin, INPUT);
    digitalWrite(trigPin, LOW);
}

float SensorUltrasonico::getDistanceCm()
{
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);

    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);

    digitalWrite(trigPin, LOW);

    // Timeout de 30 ms evita bloquear indefinidamente si no hay eco.
    unsigned long duration = pulseIn(echoPin, HIGH, 30000UL);

    if (duration == 0)
    {
        return -1;
    }

    float distance = (duration * 0.0343) / 2.0;
    return distance;
}
