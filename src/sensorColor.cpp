#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_TCS34725.h>
#include "sensorColor.h"

Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_4X);

static ColorDetectado ultimoColorReportado = NINGUNO;

void inicializarSensorColor()
{
    if (!tcs.begin())
    {
        Serial.println("ERROR: no se detecto el sensor TCS34725");
        while (1)
    }
    Serial.println("Sensor de color inicializado correctamente.");
}

void leerRGBNormalizado(float &r, float &g, float &b)
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

static float distancia(float r, float g, float b, ReferenciaColor ref)
{
    float dr = r - ref.r;
    float dg = g - ref.g;
    float db = b - ref.b;
    return sqrt(dr * dr + dg * dg + db * db);
}

ColorDetectado leerColorActual()
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

ColorDetectado detectarColorNuevo()
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

void calibrarSensorColor()
{
    uint16_t rRaw, gRaw, bRaw, cRaw;
    tcs.getRawData(&rRaw, &gRaw, &bRaw, &cRaw);

    float r, g, b;
    leerRGBNormalizado(r, g, b);

    Serial.print("RAW  r=");
    Serial.print(rRaw);
    Serial.print(" g=");
    Serial.print(gRaw);
    Serial.print(" b=");
    Serial.print(bRaw);
    Serial.print(" c=");
    Serial.print(cRaw);

    Serial.print("   NORM r=");
    Serial.print(r, 3);
    Serial.print(" g=");
    Serial.print(g, 3);
    Serial.print(" b=");
    Serial.println(b, 3);

    delay(500);
}