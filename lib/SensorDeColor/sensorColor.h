#ifndef SENSOR_COLOR_H
#define SENSOR_COLOR_H

#include <Arduino.h>
#include <Wire.h>

enum class ColorDetectado
{
    NINGUNO,
    NARANJA,
    ROJO,
    AZUL,
    VERDE,
    AMARILLO,
    ROSA
};

class SensorColor
{
private:
    uint8_t canalMultiplexor;
    const uint8_t direccionI2C = 0x29;

    ColorDetectado ultimoColor;
    ColorDetectado candidatoColor;
    uint8_t lecturasConsecutivas;
    unsigned long ultimaLectura;

    void seleccionarCanal(uint8_t canal);
    void escribirRegistro(uint8_t reg, uint8_t valor);
    uint16_t leer16bits(uint8_t reg);

public:
    SensorColor(uint8_t canal);
    void begin();
    ColorDetectado detectarColorNuevo();
    ColorDetectado detectarColorEstable();
    void leerRaw(uint16_t &c, uint16_t &r, uint16_t &g, uint16_t &b);
};

#endif