#include "sensoresUltrasonicos.h"
// varibles
// pines

// prueba
inline void iniciarlizar()
{
    Serial.begin(9600);
    Serial.println("Iniciando sensores...");
    pinMode(trig1, OUTPUT);
    pinMode(echo1, INPUT);

    pinMode(trig2, OUTPUT);
    pinMode(echo2, INPUT);

    pinMode(trig3, OUTPUT);
    pinMode(echo3, INPUT);
}

inline float medirDistancia(int trig, int echo)
{
    float distancia;

    digitalWrite(trig, LOW);
    delayMicroseconds(2);
    digitalWrite(trig, HIGH);
    delayMicroseconds(10);
    digitalWrite(trig, LOW);

    // puseIn() trabaja con el pin de input,echo, , calcula cuanto tiempo esta en HIGH y el maximo del tiempo que tarda en escuchar devuelve 0 si no detecta o esta lejos de su rango
    distancia = (0.0343 * pulseIn(echo, HIGH, 1750)) / 2;

    return distancia;
}
