/* Aqui ira la logica de la garra del robot,si los sensores como el de color y el aproximidad nos dicen que podria estar la pelota cerca entonces
se llamara a este objeto y usaremos algunas de sus funciones como cerrar o abrir garra, ademas tambien le pondre una verificacion que si detecta algo dentro de la garra
*/

#include <Arduino.h>
#include "garra.h"

Garra::Garra()
{
}

void Garra::inicializar()
{
    servo.attach(PIN_SERVO);
    abrir();
}

void Garra::abrir()
{
    servo.write(ANGULO_ABIERTA);
}

void Garra::cerrar()
{
    servo.write(ANGULO_CERRADA);
    delay(400);
}