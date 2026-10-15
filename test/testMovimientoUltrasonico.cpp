#include <Arduino.h>
#include <movimiento.h>

// Pines del puente H (los mismos de antes)
PuenteH puente(8, 9, 2, 10, 11, 3);

// Sensor ultrasónico frontal
// Cambiar estos pines cuando confirme las conexiones
SensorUltrasonico frontal(30, 31);

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
    Serial.print(String(frontal.getDistanceCm()) + "\n");
    delay(100);
}