#ifndef GARRA_H
#define GARRA_H

#include <Arduino.h>
#include <Servo.h>

class Garra
{
public:
    Garra(uint8_t pinServo);
    void inicializar(int anguloAbierta, int anguloCerrada);
    void abrir();
    void cerrar();

private:
    Servo servo;
    uint8_t pinServo;
    int anguloAbierta;
    int anguloCerrada;
};

#endif