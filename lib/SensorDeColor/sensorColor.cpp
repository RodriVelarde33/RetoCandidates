#include "sensorColor.h"
#include <math.h>

// Dirección I2C del TCS34725
#define TCS34725_ADDRESS 0x29

// Registros del TCS34725
#define COMMAND_BIT 0x80
#define ENABLE_REGISTER 0x00
#define ATIME_REGISTER 0x01
#define CONTROL_REGISTER 0x0F

#define CDATA_REGISTER 0x14
#define RDATA_REGISTER 0x16
#define GDATA_REGISTER 0x18
#define BDATA_REGISTER 0x1A

// Referencias de colores
const ReferenciaColor REF_CIAN = {0.20, 0.45, 0.35};
const ReferenciaColor REF_AMARILLO = {0.40, 0.40, 0.20};
const ReferenciaColor REF_NARANJA = {0.50, 0.30, 0.20};
const ReferenciaColor REF_MAGENTA = {0.45, 0.20, 0.35};

const float UMBRAL_DISTANCIA_MAXIMA = 0.25;


// Constructor
SensorColor::SensorColor()
{
    direccion = TCS34725_ADDRESS;
    ultimoColorReportado = NINGUNO;
}


// Configuración inicial del sensor
bool SensorColor::begin()
{
    Wire.begin();

    // Comprobar comunicación con el sensor
    Wire.beginTransmission(direccion);

    if (Wire.endTransmission() != 0)
    {
        return false;
    }

    // Tiempo de integración aproximado de 50 ms
    escribirRegistro(ATIME_REGISTER, 0xEB);

    // Ganancia 4x
    escribirRegistro(CONTROL_REGISTER, 0x01);

    // Encender sensor
    escribirRegistro(ENABLE_REGISTER, 0x01);
    delay(3);

    escribirRegistro(ENABLE_REGISTER, 0x03);
    delay(50);

    return true;
}


// Escribir un valor en un registro del sensor
void SensorColor::escribirRegistro(uint8_t registro, uint8_t valor)
{
    Wire.beginTransmission(direccion);
    Wire.write(COMMAND_BIT | registro);
    Wire.write(valor);
    Wire.endTransmission();
}


// Leer un valor de 16 bits del sensor
uint16_t SensorColor::leerRegistro16(uint8_t registro)
{
    Wire.beginTransmission(direccion);
    Wire.write(COMMAND_BIT | registro);
    Wire.endTransmission();

    Wire.requestFrom(direccion, (uint8_t)2);

    uint16_t valor = Wire.read();
    valor |= ((uint16_t)Wire.read() << 8);

    return valor;
}


// Leer y normalizar RGB
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


// Calcular distancia entre el color leído y una referencia
float SensorColor::distancia(float r, float g, float b, ReferenciaColor ref)
{
    float dr = r - ref.r;
    float dg = g - ref.g;
    float db = b - ref.b;

    return sqrt(dr * dr + dg * dg + db * db);
}


// Determinar qué color está viendo actualmente
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


// Reportar un color solamente cuando aparece por primera vez
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