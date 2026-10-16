#include "sensorGiro.h"

SensorGiro::SensorGiro(uint8_t mux, uint8_t canal, uint8_t mpu)
{
    direccionMux = mux;
    canalMux = canal;
    direccionMPU = mpu;
    giroX = giroY = giroZ = 0;
    aceleracionX = aceleracionY = aceleracionZ = 0;
}

// Selecciona el canal del multiplexor donde está el MPU6050.
 // Devuelve true si la operación fue exitosa.
bool SensorGiro::seleccionarCanal()
{
    Wire.beginTransmission(direccionMux);

    // Activa únicamente el canal indicado.
    // Por ejemplo, 1 << 2 produce 00000100, lo que selecciona el canal 2.
    Wire.write(1 << canalMux);
    return Wire.endTransmission() == 0;
}

bool SensorGiro::escribirRegistro(uint8_t registro, uint8_t valor)
{
    //Primero comprobamos que podemos comunicarnos con el multiplexor y seleccionar el canal correcto.
    if (!seleccionarCanal())
        return false;
    // Luego escribimos en el registro del MPU6050.
    Wire.beginTransmission(direccionMPU);
    Wire.write(registro);
    Wire.write(valor);
    // Un resultado igual a cero significa que no hubo error.
    return Wire.endTransmission() == 0;
}

// Lee varios registros consecutivos del MPU6050.
bool SensorGiro::leerRegistros(uint8_t registro, uint8_t cantidad, uint8_t datos[])
{
    if (!seleccionarCanal())
        return false;
    Wire.beginTransmission(direccionMPU);
    Wire.write(registro);
    if (Wire.endTransmission(false) != 0)
        return false;

    uint8_t recibidos = Wire.requestFrom(direccionMPU, cantidad, (uint8_t)true);
    if (recibidos != cantidad)
    {
         // Si faltan datos, descarta los bytes que queden pendientes.
        while (Wire.available())
            Wire.read();
        return false;
    }
    // Copia los bytes recibidos al arreglo proporcionado.
    for (uint8_t i = 0; i < cantidad; i++)
        datos[i] = Wire.read();
    return true;
}

// Comprueba la identidad del sensor y configura su funcionamiento.
bool SensorGiro::begin()
{
    uint8_t identidad[1];
    if (!leerRegistros(0x75, 1, identidad))
        return false;
    if (identidad[0] != 0x68)
        return false;
    if (!escribirRegistro(0x6B, 0x00))
        return false; // despierta el MPU
    if (!escribirRegistro(0x1A, 0x03))
        return false; // filtro pasa-bajas ~44 Hz (menos ruido)
    if (!escribirRegistro(0x1B, 0x08))
        return false; // giroscopio +-500 °/s (antes +-250: se saturaba al girar rápido)
    if (!escribirRegistro(0x1C, 0x00))
        return false; // acelerómetro +-2 g
    return true;
}

bool SensorGiro::actualizar()
{
    // El MPU6050 tiene 14 bytes consecutivos de datos:
    // 6 de aceleración, 2 de temperatura y 6 de velocidad angular.
    uint8_t datos[14];

    // Comienza a leer desde el registro 0x3B.
    if (!leerRegistros(0x3B, 14, datos))
        return false;

    // Une los dos bytes de la aceleración del eje X.
    // El primero contiene los bits más significativos
    // y el segundo contiene los menos significativos.
    int16_t ax = (int16_t)(((uint16_t)datos[0] << 8) | datos[1]);
    // Une los dos bytes de la aceleración del eje Y.
    int16_t ay = (int16_t)(((uint16_t)datos[2] << 8) | datos[3]);
    // Une los dos bytes de la aceleración del eje Z.
    int16_t az = (int16_t)(((uint16_t)datos[4] << 8) | datos[5]);

    // Los bytes 6 y 7 corresponden a la temperatura.
    // No los utilizamos en esta versión del programa.

    // Une los dos bytes de la velocidad angular del eje X.
    int16_t gx = (int16_t)(((uint16_t)datos[8] << 8) | datos[9]);
    // Une los dos bytes de la velocidad angular del eje Y.
    int16_t gy = (int16_t)(((uint16_t)datos[10] << 8) | datos[11]);
    // Une los dos bytes de la velocidad angular del eje Z.
    int16_t gz = (int16_t)(((uint16_t)datos[12] << 8) | datos[13]);

    // Convierte la aceleración original a unidades de gravedad.
    // Con el rango de +/- 2 g, cada unidad equivale a 16384 cuentas.
    aceleracionX = ax / 16384.0;
    aceleracionY = ay / 16384.0;
    aceleracionZ = az / 16384.0;
    
    giroX = gx / 65.5; // +-500 °/s => 65.5 LSB por °/s
    giroY = gy / 65.5;
    giroZ = gz / 65.5;
    return true;
}

float SensorGiro::getGiroX() { return giroX; }
float SensorGiro::getGiroY() { return giroY; }
float SensorGiro::getGiroZ() { return giroZ; }
float SensorGiro::getAceleracionX() { return aceleracionX; }
float SensorGiro::getAceleracionY() { return aceleracionY; }
float SensorGiro::getAceleracionZ() { return aceleracionZ; }