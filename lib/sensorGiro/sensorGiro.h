#ifndef SENSOR_GIROSCOPIO_H
#define SENSOR_GIROSCOPIO_H


#include <Arduino.h>
// Librería integrada de Arduino para la comunicación I2C.
// Permite leer y escribir directamente en el MPU6050.
#include <Wire.h>

class SensorGiro {

private:
    // Dirección I2C del multiplexor TCA9548A.
    uint8_t direccionMux;

    // Canal del multiplexor donde está conectado el MPU6050.
    uint8_t canalMux;

    // Dirección I2C del sensor MPU6050.
    uint8_t direccionMPU;

    // Variables para guardar la velocidad angular de cada eje.
    // Los valores se expresan en grados por segundo.
    float giroX;
    float giroY;
    float giroZ;

    // Variables para guardar la aceleración de cada eje.
    // Los valores se expresan en unidades de gravedad (g).
    float aceleracionX;
    float aceleracionY;
    float aceleracionZ;

    // Activa el canal del multiplexor correspondiente al sensor.
    // Devuelve true si la comunicación con el multiplexor funciona.
    bool seleccionarCanal();

    // Escribe un valor en un registro específico del MPU6050.
    // registro: dirección del registro que queremos modificar.
    // valor: dato que queremos escribir.
    bool escribirRegistro(uint8_t registro, uint8_t valor);

    // Lee varios registros consecutivos del MPU6050.
    // registro: dirección del primer registro que se leerá.
    // cantidad: número de bytes que queremos recibir.
    // datos: arreglo donde se guardarán los bytes recibidos.
    bool leerRegistros(uint8_t registro, uint8_t cantidad,
                       uint8_t datos[]);

public:
    // Constructor de la clase.
    // Permite indicar las direcciones y el canal utilizados.
    // Si no se especifican, utiliza los valores predeterminados.
    SensorGiro(uint8_t mux = 0x70,
                     uint8_t canal = 2,
                     uint8_t mpu = 0x68);

    // Comprueba que el sensor responde y configura sus registros.
    bool begin();

    // Lee las mediciones actuales y actualiza las variables internas.
    bool actualizar();

    // Devuelven la velocidad angular de cada eje.
    float getGiroX();
    float getGiroY();
    float getGiroZ();

    // Devuelven la aceleración medida en cada eje.
    float getAceleracionX();
    float getAceleracionY();
    float getAceleracionZ();
};

#endif