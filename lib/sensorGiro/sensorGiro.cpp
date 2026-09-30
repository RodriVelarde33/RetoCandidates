#include "sensorGiro.h"
#include <Arduino.h>

Giroscopio::Giroscopio() : referencia(0), angulo(0), ultimoTiempoMs(0)
{
}

void Giroscopio::calibrar()
{
    Serial.println("Calibrando giroscopio, no muevas el robot...");
    const int muestras = 200;
    float suma = 0;
    for (int i = 0; i < muestras; i++)
    {
        sensors_event_t a, g, temp;
        mpu.getEvent(&a, &g, &temp);
        suma += g.gyro.z; // giro sobre el piso (yaw), el q importa para girar
        delay(5);
    }
    referencia = suma / muestras;
    angulo = 0;
    ultimoTiempoMs = millis();
    Serial.print("Calibracion lista. Offset Z: ");
    Serial.println(referencia, 5);
}

float Giroscopio::anguloActual()
{
    return angulo;
}
void Giroscopio::resetAngulo()
{
    angulo = 0;
}