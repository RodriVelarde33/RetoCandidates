#include <Arduino.h>
#include <garra.h>


Garra garra(7);


void setup()
{
    Serial.begin(9600);

    garra.inicializar(110, 0);

    Serial.println("Iniciando prueba de servo motor");
}

void loop()
{
    garra.abrir();
    delay(3000);
    garra.cerrar();
    delay(3000);
}