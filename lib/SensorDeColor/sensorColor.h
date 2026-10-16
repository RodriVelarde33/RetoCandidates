#ifndef SENSOR_COLOR_H
#define SENSOR_COLOR_H

#include <Arduino.h>
#include <Wire.h>

// Definición de colores detectados
enum ColorDetectado
{
    NINGUNO,
    CIAN,
    AMARILLO,
    NARANJA,
    MAGENTA
};

// Estructura para las referencias de color
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
    uint8_t canalMUX;
    ColorDetectado ultimoColorReportado;

    void seleccionarCanal();
    void escribirRegistro(uint8_t registro, uint8_t valor);
    uint16_t leerRegistro16(uint8_t registro);
    float distancia(float r, float g, float b, ReferenciaColor ref);

public:
    SensorColor(uint8_t canal);
    bool begin();
    void leerRGBNormalizado(float &r, float &g, float &b);
    ColorDetectado leerColorActual();
    ColorDetectado detectarColorNuevo();
};

#endif