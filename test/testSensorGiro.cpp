
#include <Arduino.h>
#include <Wire.h>
#include "sensorGiro.h"


SensorGiro giroscopio(
    0x70, // Dirección del multiplexor TCA9548A
    2,    // Canal del multiplexor donde está conectado el MPU6050
    0x68  // Dirección I2C del sensor MPU6050
);

void setup() {

    // Inicia el monitor serial para mostrar los resultados.
    Serial.begin(9600);

    // Inicia la comunicación I2C del Arduino Mega.
    Wire.begin();

    // Intenta comprobar y configurar el MPU6050.
    if (giroscopio.begin()) {

        // Se muestra si la inicialización fue exitosa.
        Serial.println("Giroscopio conectado correctamente");

    } else {

        // Se muestra si no se pudo inicializar el sensor.
        Serial.println("Error al conectar el Giroscopio");
    }
}

void loop() {

    // Intenta obtener nuevas mediciones del sensor.
    if (giroscopio.actualizar()) {

        // Muestra la velocidad angular del eje X.
        Serial.print("Giro X: ");
        Serial.print(giroscopio.getGiroX());

        // Muestra la velocidad angular del eje Y.
        Serial.print(" | Giro Y: ");
        Serial.print(giroscopio.getGiroY());

        // Muestra la velocidad angular del eje Z.
        Serial.print(" | Giro Z: ");
        Serial.println(giroscopio.getGiroZ());

    } else {

        // Informa si hubo un problema al leer los registros.
        Serial.println("Error al leer el giroscopio");
    }

    // Espera 200 milisegundos antes de la siguiente lectura.
    delay(200);
}