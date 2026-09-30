#ifndef SENSOR_COLOR_H
#define SENSOR_COLOR_H

#include <Adafruit_TCS34725.h>

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
    float r, g, b;
};

// Reemplaza estos con tus valores reales calibrados
const ReferenciaColor REF_CIAN = {0.20, 0.45, 0.35};
const ReferenciaColor REF_AMARILLO = {0.40, 0.40, 0.20};
const ReferenciaColor REF_NARANJA = {0.50, 0.30, 0.20};
const ReferenciaColor REF_MAGENTA = {0.45, 0.20, 0.35};
#define UMBRAL_DISTANCIA_MAXIMA 0.25

class SensorColor
{
public:
    SensorColor();

    // Regresa false si el sensor no respondio por I2C
    bool inicializar();

    // Unico metodo que vas a usar en el loop: regresa el color SOLO
    // la primera vez que aparece. Mientras siga viendo el mismo color,
    // regresa NINGUNO. Esto es lo que vas a conectar al LED RGB despues.
    ColorDetectado detectarColorNuevo();

private:
    Adafruit_TCS34725 tcs;
    ColorDetectado ultimoColorReportado;

    void leerRGBNormalizado(float &r, float &g, float &b);
    ColorDetectado leerColorActual();
    float distancia(float r, float g, float b, ReferenciaColor ref);
};

#endif