#include "sensorUltrasonico.h"

//Constructor
SensorUltrasonico::SensorUltrasonico(uint8_t trig, uint8_t echo)
{
    trigPin = trig;
    echoPin = echo;
}


// Configuración inicial del sensor
void SensorUltrasonico::begin() {
    pinMode(trigPin, OUTPUT);
    pinMode(echoPin, INPUT);

    digitalWrite(trigPin, LOW);
}

float SensorUltrasonico::getDistanceCm() {
    // Enviar un pulso ultrasónico de 10 microsegundos
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);

    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);

    digitalWrite(trigPin, LOW);

    // Medir el tiempo que tarda en recibir el eco
    unsigned long duration = pulseIn(echoPin, HIGH, 30000); // Timeout de 30 ms para evitar bloqueos

    if (duration == 0) { // Si no se recibe eco, retornar -1 para indicar error/no hay pared detectada
        return -1;
    }

    // Calcular la distancia en centímetros
    float distance = (duration * 0.0343) / 2.0; // Velocidad del sonido: 343 m/s

    return distance;
}

