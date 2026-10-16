#include "sensorGiro.h"

SensorGiro::SensorGiro(uint8_t mux, uint8_t canal, uint8_t mpu)
{
    direccionMux = mux;
    canalMux = canal;
    direccionMPU = mpu;
    giroX = giroY = giroZ = 0;
    aceleracionX = aceleracionY = aceleracionZ = 0;
}

bool SensorGiro::seleccionarCanal()
{
    Wire.beginTransmission(direccionMux);
    Wire.write(1 << canalMux);
    return Wire.endTransmission() == 0;
}

bool SensorGiro::escribirRegistro(uint8_t registro, uint8_t valor)
{
    if (!seleccionarCanal())
        return false;
    Wire.beginTransmission(direccionMPU);
    Wire.write(registro);
    Wire.write(valor);
    return Wire.endTransmission() == 0;
}

bool SensorGiro::leerRegistros(uint8_t registro, uint8_t cantidad, uint8_t datos[])
{
    if (!seleccionarCanal())
        return false;
    Wire.beginTransmission(direccionMPU);
    Wire.write(registro);
    if (Wire.endTransmission(false) != 0)
        return false;

    uint8_t recibidos = Wire.requestFrom(direccionMPU, cantidad, (uint8_t)true);
    if (recibidos != cantidad)
    {
        while (Wire.available())
            Wire.read();
        return false;
    }
    for (uint8_t i = 0; i < cantidad; i++)
        datos[i] = Wire.read();
    return true;
}

bool SensorGiro::begin()
{
    uint8_t identidad[1];
    if (!leerRegistros(0x75, 1, identidad))
        return false;
    if (identidad[0] != 0x68)
        return false;
    if (!escribirRegistro(0x6B, 0x00))
        return false; // despierta el MPU
    if (!escribirRegistro(0x1A, 0x03))
        return false; // filtro pasa-bajas ~44 Hz (menos ruido)
    if (!escribirRegistro(0x1B, 0x08))
        return false; // giroscopio +-500 °/s (antes +-250: se saturaba al girar rápido)
    if (!escribirRegistro(0x1C, 0x00))
        return false; // acelerómetro +-2 g
    return true;
}

bool SensorGiro::actualizar()
{
    uint8_t datos[14];
    if (!leerRegistros(0x3B, 14, datos))
        return false;

    int16_t ax = (int16_t)(((uint16_t)datos[0] << 8) | datos[1]);
    int16_t ay = (int16_t)(((uint16_t)datos[2] << 8) | datos[3]);
    int16_t az = (int16_t)(((uint16_t)datos[4] << 8) | datos[5]);
    int16_t gx = (int16_t)(((uint16_t)datos[8] << 8) | datos[9]);
    int16_t gy = (int16_t)(((uint16_t)datos[10] << 8) | datos[11]);
    int16_t gz = (int16_t)(((uint16_t)datos[12] << 8) | datos[13]);

    aceleracionX = ax / 16384.0;
    aceleracionY = ay / 16384.0;
    aceleracionZ = az / 16384.0;
    giroX = gx / 65.5; // +-500 °/s => 65.5 LSB por °/s
    giroY = gy / 65.5;
    giroZ = gz / 65.5;
    return true;
}

float SensorGiro::getGiroX() { return giroX; }
float SensorGiro::getGiroY() { return giroY; }
float SensorGiro::getGiroZ() { return giroZ; }
float SensorGiro::getAceleracionX() { return aceleracionX; }
float SensorGiro::getAceleracionY() { return aceleracionY; }
float SensorGiro::getAceleracionZ() { return aceleracionZ; }