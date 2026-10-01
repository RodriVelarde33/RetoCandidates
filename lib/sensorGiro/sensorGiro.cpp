#include <Arduino.h>
#include <Wire.h>
#include "giroscopio.h"

Giroscopio::Giroscopio()
    : referencia(0), angulo(0), ultimoTiempoMs(0)
{
}

bool Giroscopio::inicializar()
{
    if (!mpu.begin())
    {
        Serial.println("ERROR: no se detecto el MPU6050");
        return false;
    }

    mpu.setGyroRange(MPU6050_RANGE_250_DEG);
    mpu.setFilterBandwidth(MPU6050_BAND_21_HZ); // suaviza  el ruido

    Serial.println("MPU6050 inicializado correctamente.");
    return true;
}

void Giroscopio::calibrar()
{
    Serial.println("Calibrando giroscopio, no mueva el robot...");

    const int muestras = 200;
    float suma = 0;

    for (int i = 0; i < muestras; i++)
    {
        sensors_event_t a, g, temp;
        mpu.getEvent(&a, &g, &temp);
        suma += g.gyro.z; // eje Z = giro sobre el piso (yaw), el q importa para girar
        delay(5);
    }

    referencia = suma / muestras;
    angulo = 0;
    ultimoTiempoMs = millis();

    Serial.print("Calibracion lista. desde Z: ");
    Serial.println(referencia, 5);
}

void Giroscopio::actualizar()
{
    unsigned long ahora = millis();

    if (ultimoTiempoMs == 0)
    {
        ultimoTiempoMs = ahora;
        return; // primera llamada
    }

    float dt = (ahora - ultimoTiempoMs) / 1000.0; // de milisegundos a segundos
    ultimoTiempoMs = ahora;

    sensors_event_t a, g, temp;
    mpu.getEvent(&a, &g, &temp);

    // g.gyro.z viene en radianes/segundo, le restamos el lareferencia calibrada
    float velZRadPorSeg = g.gyro.z - referencia;
    float velZGradosPorSeg = velZRadPorSeg * 180.0 / PI;

    angulo += velZGradosPorSeg * dt; // aqui se acumulael giro, poquito a poquito
}

float Giroscopio::anguloActual()
{
    return angulo;
}

void Giroscopio::resetAngulo()
{
    angulo = 0;
}