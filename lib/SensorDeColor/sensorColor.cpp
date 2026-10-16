#include "sensorColor.h"
#include "../logica/config.h"

// Registros TCS34725
#define TCS34725_COMMAND_BIT 0x80
#define TCS34725_ENABLE 0x00
#define TCS34725_ATIME 0x01
#define TCS34725_CONTROL 0x0F
#define TCS34725_ID 0x12
#define TCS34725_CDATAL 0x14
#define TCS34725_RDATAL 0x16
#define TCS34725_GDATAL 0x18
#define TCS34725_BDATAL 0x1A

// Selecciona canal del multiplexor
void SensorColor::seleccionarCanal(uint8_t canal)
{
    Wire.beginTransmission(0x70);
    Wire.write(1 << canal);
    Wire.endTransmission();
}

// Escribe registro
void SensorColor::escribirRegistro(uint8_t reg, uint8_t valor)
{
    Wire.beginTransmission(direccionI2C);
    Wire.write(TCS34725_COMMAND_BIT | reg);
    Wire.write(valor);
    Wire.endTransmission();
}

// Lee 16 bits (low + high)
uint16_t SensorColor::leer16bits(uint8_t reg)
{
    Wire.beginTransmission(direccionI2C);
    Wire.write(TCS34725_COMMAND_BIT | reg);
    Wire.endTransmission();
    Wire.requestFrom(direccionI2C, (uint8_t)2);
    if (Wire.available() >= 2)
    {
        uint16_t low = Wire.read();
        uint16_t high = Wire.read();
        return (high << 8) | low;
    }
    return 0;
}

// Constructor
SensorColor::SensorColor(uint8_t canal) : canalMultiplexor(canal)
{
    ultimoColor = ColorDetectado::NINGUNO;
    candidatoColor = ColorDetectado::NINGUNO;
    lecturasConsecutivas = 0;
    ultimaLectura = 0;
}

// Init sensor
void SensorColor::begin()
{
    seleccionarCanal(canalMultiplexor);
    escribirRegistro(TCS34725_ENABLE, 0x01);
    delay(3);
    escribirRegistro(TCS34725_ENABLE, 0x03);
    escribirRegistro(TCS34725_ATIME, 0xEB);
    escribirRegistro(TCS34725_CONTROL, 0x01);
}

// Lee valores crudos C,R,G,B
void SensorColor::leerRaw(uint16_t &c, uint16_t &r, uint16_t &g, uint16_t &b)
{
    seleccionarCanal(canalMultiplexor);
    c = leer16bits(TCS34725_CDATAL);
    r = leer16bits(TCS34725_RDATAL);
    g = leer16bits(TCS34725_GDATAL);
    b = leer16bits(TCS34725_BDATAL);
}

// Clasifica color con umbrales calibrados
ColorDetectado SensorColor::detectarColorNuevo()
{
    uint16_t c, r, g, b;
    leerRaw(c, r, g, b);

    if (r > 200 && g < 100 && b < 100)
        return ColorDetectado::ROJO;
    else if (r > 300 && g > 120 && g < 200 && b < 100)
        return ColorDetectado::NARANJA;
    else if (r > 400 && g > 300 && r > g && b < 200)
        return ColorDetectado::AMARILLO;
    else if (r > 500 && b > 180 && g > 150)
        return ColorDetectado::ROSA;
    else if (r < 150 && g > 160 && b > 160)
        return ColorDetectado::AZUL;
    else if (g > r && g > b && c < 300)
        return ColorDetectado::VERDE;

    return ColorDetectado::NINGUNO;
}

// Antirrebote: N lecturas consecutivas
ColorDetectado SensorColor::detectarColorEstable()
{
    unsigned long ahora = millis();

    if (ahora - ultimaLectura < COLOR_INTERVALO_MS)
    {
        return ultimoColor;
    }

    ultimaLectura = ahora;

    ColorDetectado lectura = detectarColorNuevo();

    if (lectura == candidatoColor)
    {
        if (lecturasConsecutivas < 255)
            lecturasConsecutivas++;
    }
    else
    {
        candidatoColor = lectura;
        lecturasConsecutivas = 1;
    }

    if (lecturasConsecutivas >= COLOR_LECTURAS_CONSECUTIVAS)
    {
        ultimoColor = candidatoColor;
        lecturasConsecutivas = 0;
    }

    return ultimoColor;
}