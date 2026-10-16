#include "sensorUltrasonico.h"
#include "../logica/config.h"

SensorUltrasonico::SensorUltrasonico(uint8_t trig, uint8_t echo)
{
    trigPin = trig;
    echoPin = echo;

    indiceBuffer = 0;
    muestrasValidas = 0;
    ultimaDistancia = -1.0f;
    ultimaMedicion = 0;

    for (uint8_t i = 0; i < BUFFER_SIZE; i++)
    {
        buffer[i] = -1.0f;
    }
}

void SensorUltrasonico::begin()
{
    pinMode(trigPin, OUTPUT);
    pinMode(echoPin, INPUT);
    digitalWrite(trigPin, LOW);
}

void SensorUltrasonico::disparar()
{
    digitalWrite(trigPin, LOW);
    delayMicroseconds(4);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
}

float SensorUltrasonico::leerPulsoCm()
{
    unsigned long duracion = pulseIn(echoPin, HIGH, ULTRASONICO_TIMEOUT_US);

    if (duracion == 0)
        return -1.0f;

    float distancia = (duracion * 0.0343f) / 2.0f;

    if (distancia < ULTRASONICO_MIN_CM ||
        distancia > ULTRASONICO_MAX_CM)
    {
        return -1.0f;
    }

    return distancia;
}

float SensorUltrasonico::calcularMediana()
{
    float ordenados[BUFFER_SIZE];
    uint8_t validos = 0;

    for (uint8_t i = 0; i < muestrasValidas; i++)
    {
        if (buffer[i] > 0.0f)
        {
            ordenados[validos++] = buffer[i];
        }
    }

    if (validos == 0)
        return -1.0f;

    for (uint8_t i = 1; i < validos; i++)
    {
        float clave = ordenados[i];
        int8_t j = i - 1;
        while (j >= 0 && ordenados[j] > clave)
        {
            ordenados[j + 1] = ordenados[j];
            j--;
        }
        ordenados[j + 1] = clave;
    }

    return ordenados[validos / 2];
}

void SensorUltrasonico::update(unsigned long ahora)
{
    if (ultimaMedicion != 0 &&
        ahora - ultimaMedicion < ULTRASONICO_INTERVALO_MS)
    {
        return;
    }

    ultimaMedicion = ahora;

    disparar();
    float distancia = leerPulsoCm();

    buffer[indiceBuffer] = distancia;
    indiceBuffer = (indiceBuffer + 1) % BUFFER_SIZE;

    if (muestrasValidas < BUFFER_SIZE)
        muestrasValidas++;

    ultimaDistancia = calcularMediana();
}

float SensorUltrasonico::getDistanceCm()
{
    return ultimaDistancia;
}