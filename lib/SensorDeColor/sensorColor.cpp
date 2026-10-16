#include "sensorColor.h"
#include <math.h>

#define TCS34725_ADDRESS 0x29
#define MUX_DIRECCION 0x70 // Dirección I2C del multiplexor TCA9548A

#define COMMAND_BIT 0x80
#define ENABLE_REGISTER 0x00
#define ATIME_REGISTER 0x01
#define CONTROL_REGISTER 0x0F
#define CDATA_REGISTER 0x14
#define RDATA_REGISTER 0x16
#define GDATA_REGISTER 0x18
#define BDATA_REGISTER 0x1A

// Referencias de color configuradas para tu pista
const ReferenciaColor REF_CIAN = {0.20f, 0.45f, 0.35f};
const ReferenciaColor REF_AMARILLO = {0.40f, 0.40f, 0.20f};
const ReferenciaColor REF_NARANJA = {0.50f, 0.30f, 0.20f};
const ReferenciaColor REF_MAGENTA = {0.45f, 0.20f, 0.35f};

const float UMBRAL_DISTANCIA_MAXIMA = 0.25f;

// Constructor que recibe el canal asignado (ej. 3 para Piso, 4 para Frente)
SensorColor::SensorColor(uint8_t canal)
{
    direccion = TCS34725_ADDRESS;
    canalMUX = canal;
    ultimoColorReportado = NINGUNO;
}

// Función interna para abrir el canal en el multiplexor
void SensorColor::seleccionarCanal()
{
    if (canalMUX > 7)
        return;
    Wire.beginTransmission(MUX_DIRECCION);
    Wire.write(1 << canalMUX);
    Wire.endTransmission();
}

bool SensorColor::begin()
{
    seleccionarCanal();

    Wire.beginTransmission(direccion);
    if (Wire.endTransmission() != 0)
    {
        return false;
    }

    escribirRegistro(ATIME_REGISTER, 0xEB);
    escribirRegistro(CONTROL_REGISTER, 0x01);
    escribirRegistro(ENABLE_REGISTER, 0x01);
    delay(3);
    escribirRegistro(ENABLE_REGISTER, 0x03);
    delay(50);

    return true;
}

void SensorColor::escribirRegistro(uint8_t registro, uint8_t valor)
{
    seleccionarCanal();
    Wire.beginTransmission(direccion);
    Wire.write(COMMAND_BIT | registro);
    Wire.write(valor);
    Wire.endTransmission();
}

uint16_t SensorColor::leerRegistro16(uint8_t registro)
{
    seleccionarCanal();
    Wire.beginTransmission(direccion);
    Wire.write(COMMAND_BIT | registro);
    Wire.endTransmission();

    Wire.requestFrom(direccion, (uint8_t)2);
    if (Wire.available() < 2)
        return 0;
    uint16_t valor = Wire.read();
    valor |= ((uint16_t)Wire.read() << 8);

    return valor;
}

void SensorColor::leerRGBNormalizado(float &r, float &g, float &b)
{
    uint16_t cRaw = leerRegistro16(CDATA_REGISTER);
    uint16_t rRaw = leerRegistro16(RDATA_REGISTER);
    uint16_t gRaw = leerRegistro16(GDATA_REGISTER);
    uint16_t bRaw = leerRegistro16(BDATA_REGISTER);

    if (cRaw == 0)
    {
        r = 0;
        g = 0;
        b = 0;
        return;
    }

    r = (float)rRaw / cRaw;
    g = (float)gRaw / cRaw;
    b = (float)bRaw / cRaw;
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
        return NINGUNO;
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