#ifndef SENSOR_COLOR_H
#define SENSOR_COLOR_H

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

const ReferenciaColor REF_CIAN = {0.20, 0.45, 0.35};
const ReferenciaColor REF_AMARILLO = {0.40, 0.40, 0.20};
const ReferenciaColor REF_NARANJA = {0.50, 0.30, 0.20};
const ReferenciaColor REF_MAGENTA = {0.45, 0.20, 0.35};
#define UMBRAL_DISTANCIA_MAXIMA 0.25

void inicializarSensorColor();

void leerRGBNormalizado(float &r, float &g, float &b);

ColorDetectado leerColorActual();

ColorDetectado detectarColorNuevo();

void calibrarSensorColor();

#endif