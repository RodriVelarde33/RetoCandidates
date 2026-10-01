#include "sensorColor.h"
#include <math.h>

//Constantes para el sensor de color TCS34725

// Dirección I2C del TCS34725
#define TCS34725_ADDRESS 0x29

// Registros del TCS34725

// Bit que se agrega a los registros para indicarle al TCS34725
// que le estamos enviando un comando.
#define COMMAND_BIT 0x80
#define ENABLE_REGISTER 0x00 // Registro que controla si el sensor esta encendido.
#define ATIME_REGISTER 0x01 // Registro que controla el tiempo durante el cual el sensor toma una lectura de color.
#define CONTROL_REGISTER 0x0F // Registro que controla la ganancia o sensibilidad del sensor.
#define CDATA_REGISTER 0x14 // Registro donde comienza el valor de luz total (Clear).
#define RDATA_REGISTER 0x16 // Registro donde comienza el valor de luz roja.
#define GDATA_REGISTER 0x18 // Registro donde comienza el valor de luz verde.
#define BDATA_REGISTER 0x1A // Registro donde comienza el valor de luz azul.

// Referencias de colores RGB
const ReferenciaColor REF_CIAN = {0.20, 0.45, 0.35};
const ReferenciaColor REF_AMARILLO = {0.40, 0.40, 0.20};
const ReferenciaColor REF_NARANJA = {0.50, 0.30, 0.20};
const ReferenciaColor REF_MAGENTA = {0.45, 0.20, 0.35};

// Distancia maxima permitida entre una lectura y un color de referencia para considerarlos iguales.
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

    // endTransmission() devuelve 0 cuando el dispositivo respondio correctamente
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


// Leer un valor de 16 bits - 2 bytes del sensor
uint16_t SensorColor::leerRegistro16(uint8_t registro)
{
    Wire.beginTransmission(direccion);
    Wire.write(COMMAND_BIT | registro);
    Wire.endTransmission();

    Wire.requestFrom(direccion, (uint8_t)2); // Leer 2 bytes del registro

    uint16_t valor = Wire.read(); // Leer el byte menos significativo
    valor |= ((uint16_t)Wire.read() << 8); // Leer el byte más significativo y combinarlo con el anterior

    return valor;
}


// Leer y normalizar RGB
void SensorColor::leerRGBNormalizado(float &r, float &g, float &b)
{
    // Leer valores crudos de los registros del sensor
    uint16_t cRaw = leerRegistro16(CDATA_REGISTER);
    uint16_t rRaw = leerRegistro16(RDATA_REGISTER);
    uint16_t gRaw = leerRegistro16(GDATA_REGISTER);
    uint16_t bRaw = leerRegistro16(BDATA_REGISTER);

    // Si el valor de C (luminosidad) es 0, todos los demás valores son 0
    if (cRaw == 0)
    {
        r = 0;
        g = 0;
        b = 0;
        return;
    }

    //Dividimos cada componente entre la luz total. Esto normaliza los valores y reduce el efectode cambios en la intensidad de iluminacion.
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

    // Calculamos la distancia entre ambos colores, mientras menor sea este resultado, más parecido es el color detectado al de referencia.
    return sqrt(dr * dr + dg * dg + db * db);
}


// Determinar qué color está viendo actualmente
ColorDetectado SensorColor::leerColorActual()
{
    // Variables donde guardaremos el RGB normalizado.
    float r, g, b;

    // Obtenemos la lectura actual del sensor.
    leerRGBNormalizado(r, g, b);

    //Caclulamos la distancia entre el color detectado y cada color de referencia.
    float dCian = distancia(r, g, b, REF_CIAN);
    float dAmarillo = distancia(r, g, b, REF_AMARILLO);
    float dNaranja = distancia(r, g, b, REF_NARANJA);
    float dMagenta = distancia(r, g, b, REF_MAGENTA);

    // Empezamos suponiendo que CIAN es el color mas cercano.
    float minDist = dCian;
    ColorDetectado resultado = CIAN;

    //Ir checando cual color tiene la minima de distancia al detectado
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

    //Checar que la minima distancia sea menor al umbral, si no es asi, significa que el color detectado no es ninguno de los colores de referencia.
    if (minDist > UMBRAL_DISTANCIA_MAXIMA)
    {
        return NINGUNO;
    }

    return resultado;
}


// Reportar un color solamente cuando aparece por primera vez
ColorDetectado SensorColor::detectarColorNuevo()
{
    // Obtenemos el color que el sensor esta viendo ahora.
    ColorDetectado actual = leerColorActual();

    //Si el color actual es diferente al ultimo color reportado, significa que es un nuevo color, por lo que lo reportamos.
    if (actual != NINGUNO && actual != ultimoColorReportado)
    {
        //updateamos el ultimo color reportado y retornamos el nuevo color detectado.
        ultimoColorReportado = actual;
        return actual;
    }

    if (actual == NINGUNO)
    {
        ultimoColorReportado = NINGUNO;
    }

    return NINGUNO;
}