#include <Arduino.h>
#include <movimiento.h>
#include <sensorColor.h>
#include <sensorUltrasonico.h>
#include <moduloPuenteH.h>

// Pines del puente H (los mismos de antes)
PuenteH puente(8, 9, 2, 10, 11, 3);

// Sensor ultrasónico frontal
// Cambiar estos pines cuando confirme las conexiones
SensorUltrasonico frontal(42, 44);

SensorColor colorFrontal(); // Pines para el sensor de color

// Integrar los componentes
Movimiento movimiento(puente, frontal);

void setup()
{
    Serial.begin(9600);

    movimiento.begin();

    Serial.println("Iniciando prueba de movimiento");
}

void loop()
{
    movimiento.actualizar();

    delay(100);
}