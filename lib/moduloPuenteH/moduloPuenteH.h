#ifndef PUENTE_H_H
#define PUENTE_H_H

#include <Arduino.h>

class PuenteH
{
private:
    uint8_t in1, in2, ena;
    uint8_t in3, in4, enb;

    float factorIzq = 1.0;
    float factorDer = 1.0;

public:
    PuenteH(uint8_t in1, uint8_t in2, uint8_t ena,
            uint8_t in3, uint8_t in4, uint8_t enb);

    void begin();
    void setFactores(float izq, float der);

    void izquierdaAdelante(uint8_t velocidad);
    void izquierdaAtras(uint8_t velocidad);
    void derechaAdelante(uint8_t velocidad);
    void derechaAtras(uint8_t velocidad);

    void detenerIzquierda();
    void detenerDerecha();
    void detener();
};

#endif