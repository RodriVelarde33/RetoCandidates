#ifndef SENSOR_GIROSCOPIO_H
#define SENSOR_GIROSCOPIO_H

#include <Arduino.h>
#include <Wire.h>

class SensorGiro
{
private:
    uint8_t direccionMux;
    uint8_t canalMux;
    uint8_t direccionMPU;

    float giroX, giroY, giroZ;
    float aceleracionX, aceleracionY, aceleracionZ;

    bool seleccionarCanal();
    bool escribirRegistro(uint8_t registro, uint8_t valor);
    bool leerRegistros(uint8_t registro, uint8_t cantidad, uint8_t datos[]);

public:
    SensorGiro(uint8_t mux = 0x70, uint8_t canal = 2, uint8_t mpu = 0x68);

    bool begin();
    bool actualizar();

    float getGiroX();
    float getGiroY();
    float getGiroZ();
    float getAceleracionX();
    float getAceleracionY();
    float getAceleracionZ();
};

#endif