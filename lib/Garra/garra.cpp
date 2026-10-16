#include <Arduino.h>
#include "garra.h"

Garra::Garra(uint8_t pinServo)
{
    this->pinServo = pinServo;
}

void Garra::inicializar(int anguloAbierta, int anguloCerrada)
{
    this->anguloAbierta = anguloAbierta;
    this->anguloCerrada = anguloCerrada;
    servo.attach(pinServo);
    abrir();
}

void Garra::abrir()
{
    servo.write(anguloAbierta);
}

void Garra::cerrar()
{
    servo.write(anguloCerrada);
    delay(400);
}