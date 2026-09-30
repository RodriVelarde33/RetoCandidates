#include <Arduino.h>
#include <Wire.h>
#include "sensorColor.h"

SensorColor::SensorColor()
    : tcs(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_4X),
      ultimoColorReportado(NINGUNO)
{
}

bool SensorColor::inicializar()
{
    if (!tcs.begin())
    {
        Serial.println("ERROR: no se detecto el sensor TCS34725");
        return false;
    }
    Serial.println("Sensor de color inicializado correctamente.");
    return true;
}

void SensorColor::leerRGBNormalizado(float &r, float &g, float &b)
{
    uint16_t rRaw, gRaw, bRaw, cRaw;
    tcs.getRawData(&rRaw, &gRaw, &bRaw, &cRaw);

    if (cRaw == 0)
    {
        r = g = b = 0;
        return;
    }

    r = (float)rRaw / (float)cRaw;
    g = (float)gRaw / (float)cRaw;
    b = (float)bRaw / (float)cRaw;
}

float SensorColor::distancia(float r, float g, float b, ReferenciaColor ref)
{
    float dr = r - ref.r;
    float dg = g - ref.g;
    float db = b - ref.b;
    return sqrt(dr * dr + dg * dg + db * db);
}

ColorDetectado SensorColor::leerColorActual()
{
    float r, g, b;
    leerRGBNormalizado(r, g, b);

    float dCian = distancia(r, g, b, REF_CIAN);
    float dAmarillo = distancia(r, g, b, REF_AMARILLO);
    float dNaranja = distancia(r, g, b, REF_NARANJA);
    float dMagenta = distancia(r, g, b, REF_MAGENTA);

    float minDist = dCian;
    ColorDetectado resultado = CIAN;

    if (dAmarillo < minDist)
    {
        minDist = dAmarillo;
        resultado = AMARILLO;
    }
    if (dNaranja < minDist)
    {
        minDist = dNaranja;
        resultado = NARANJA;
    }
    if (dMagenta < minDist)
    {
        minDist = dMagenta;
        resultado = MAGENTA;
    }

    if (minDist > UMBRAL_DISTANCIA_MAXIMA)
    {
        return NINGUNO;
    }

    return resultado;
}

ColorDetectado SensorColor::detectarColorNuevo()
{
    ColorDetectado actual = leerColorActual();

    if (actual != NINGUNO && actual != ultimoColorReportado)
    {
        ultimoColorReportado = actual;
        return actual;
    }

    if (actual == NINGUNO)
    {
        ultimoColorReportado = NINGUNO;
    }

    return NINGUNO;
}