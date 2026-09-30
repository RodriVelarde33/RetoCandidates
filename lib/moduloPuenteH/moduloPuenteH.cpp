#include "moduloPuenteH.h"

PuenteH::PuenteH(uint8_t in1, uint8_t in2, uint8_t ena,
                 uint8_t in3, uint8_t in4, uint8_t enb)
{
    this->in1 = in1;
    this->in2 = in2;
    this->ena = ena;

    this->in3 = in3;
    this->in4 = in4;
    this->enb = enb;
}

void PuenteH::begin()
{
    // Configurar pines
}

void PuenteH::izquierdaAdelante(uint8_t velocidad)
{
    // Implementar después
}

void PuenteH::izquierdaAtras(uint8_t velocidad)
{
    // Implementar después
}

void PuenteH::detenerIzquierda()
{
    // Implementar después
}

void PuenteH::derechaAdelante(uint8_t velocidad)
{
    // Implementar después
}

void PuenteH::derechaAtras(uint8_t velocidad)
{
    // Implementar después
}

void PuenteH::detenerDerecha()
{
    // Implementar después
}

void PuenteH::detener()
{
    // Implementar después
}