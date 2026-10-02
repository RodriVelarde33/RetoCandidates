#include "../lib/sensorGiro/sensorGiro.h"
// voy a probar esto cuando llegue el sensor de giro, por el momento como no ocupa datos como pines del arduino se puede programar la logica y ya al final solo se conecta
Giroscopio giro;

void setup()
{
    Serial.begin(9600);
    while (!Serial)
        ;

    if (!giro.inicializar())
    {
        while (1)
            ; // se detiene si no encuentra el sensor
    }

    giro.calibrar(); // ROBOT QUIETO mientras esto corre
}

void loop()
{
    giro.actualizar(); //

    Serial.println(giro.anguloActual()); // para verificar que funciona
}