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
    //Motor izquierdo
    pinMode(ena, OUTPUT);
    pinMode(in1, OUTPUT);
    pinMode(in2, OUTPUT);

    // Motor derecho
    pinMode(enb, OUTPUT);
    pinMode(in3, OUTPUT);
    pinMode(in4, OUTPUT);
}

void PuenteH::izquierdaAdelante(uint8_t velocidad)
{
    analogWrite(ena, velocidad);
    digitalWrite(in1, HIGH);
    digitalWrite(in2, LOW);
}

void PuenteH::izquierdaAtras(uint8_t velocidad)
{
    analogWrite(ena, velocidad);
    digitalWrite(in1, LOW);
    digitalWrite(in2, HIGH);
}

void PuenteH::detenerIzquierda()
{
   analogWrite(ena, 0);
   digitalWrite(in1, LOW);
digitalWrite(in2, LOW);
}

void PuenteH::derechaAdelante(uint8_t velocidad)
{
    analogWrite(enb, velocidad);
    digitalWrite(in3, HIGH);
    digitalWrite(in4, LOW);
}

void PuenteH::derechaAtras(uint8_t velocidad)
{
    analogWrite(enb, velocidad);
    digitalWrite(in3, LOW);
    digitalWrite(in4, HIGH);
}

void PuenteH::detenerDerecha()
{
    analogWrite(enb, 0);
    digitalWrite(in3, LOW);
    digitalWrite(in4, LOW);
}

void PuenteH::detener()
{
    // Velocidad 0
    analogWrite(ena, 0);
    analogWrite(enb, 0);

    // Desactivar señales de direccion
    digitalWrite(in1, LOW);
    digitalWrite(in2, LOW);

    digitalWrite(in3, LOW);
    digitalWrite(in4, LOW);
}