#ifndef PUENTE_H_H
#define PUENTE_H_H

#include <Arduino.h>

class PuenteH
{
private:
    // Motor A
    uint8_t in1;
    uint8_t in2;
    uint8_t ena;

    // Motor B
    uint8_t in3;
    uint8_t in4;
    uint8_t enb;

public:
    PuenteH(uint8_t in1, uint8_t in2, uint8_t ena, uint8_t in3, uint8_t in4, uint8_t enb);

    void begin();

    void izquierdaAdelante(uint8_t velocidad);
    void izquierdaAtras(uint8_t velocidad);

    void derechaAdelante(uint8_t velocidad);
    void derechaAtras(uint8_t velocidad);

    void detenerIzquierda();
    void detenerDerecha();
    void detener();
};

#endif