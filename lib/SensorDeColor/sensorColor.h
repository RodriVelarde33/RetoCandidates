#ifndef SENSOR_COLOR_H
#define SENSOR_COLOR_H

#include <Arduino.h>
#include <Wire.h>

enum ColorDetectado
{
    NINGUNO,
    CIAN,
    AMARILLO,
    NARANJA,
    MAGENTA
};

struct ReferenciaColor
{
    float r;
    float g;
    float b;
};

class SensorColor
{
private:
    uint8_t direccion;
    ColorDetectado ultimoColorReportado;

    void leerRGBNormalizado(float &r, float &g, float &b);
    ColorDetectado leerColorActual();
    float distancia(float r, float g, float b, ReferenciaColor ref);

    void escribirRegistro(uint8_t registro, uint8_t valor);
    uint16_t leerRegistro16(uint8_t registro);

public:
    SensorColor();

    bool begin();
    ColorDetectado detectarColorNuevo();
};

#endif