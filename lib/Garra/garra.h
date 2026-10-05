#ifndef GARRA_H
#define GARRA_H

#include <Servo.h>

class Garra
{
public:
    Garra();

    void inicializar();

    // mueve el servo para abrir la garra
    void abrir();

    // mueve el servo para cerrar la garra
    void cerrar();

private:
    Servo servo;

    // pin para el servo de la garra
    static const int PIN_SERVO = 9;

    // angulos calibrados para abrir y cerrar la garra
    static const int ANGULO_ABIERTA = 90;
    static const int ANGULO_CERRADA = 10;
};

#endif