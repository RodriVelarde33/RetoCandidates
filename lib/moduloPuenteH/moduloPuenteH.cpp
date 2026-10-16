#include "moduloPuenteH.h"

PuenteH::PuenteH(uint8_t in1, uint8_t in2, uint8_t ena,
                 uint8_t in3, uint8_t in4, uint8_t enb)
{
    this->in1 = in3;
    this->in2 = in4;
    this->ena = enb;

    this->in3 = in1;
    this->in4 = in2;
    this->enb = ena;
}

void PuenteH::begin()
{
    pinMode(ena, OUTPUT);
    pinMode(in1, OUTPUT);
    pinMode(in2, OUTPUT);
    pinMode(enb, OUTPUT);
    pinMode(in3, OUTPUT);
    pinMode(in4, OUTPUT);

    digitalWrite(ena, LOW);
    digitalWrite(in1, LOW);
    digitalWrite(in2, LOW);
    digitalWrite(enb, LOW);
    digitalWrite(in3, LOW);
    digitalWrite(in4, LOW);
}

void PuenteH::setFactores(float izq, float der)
{
    factorIzq = izq;
    factorDer = der;
}

void PuenteH::izquierdaAdelante(uint8_t v)
{
    analogWrite(ena, (uint8_t)(v * factorIzq));
    digitalWrite(in1, HIGH);
    digitalWrite(in2, LOW);
}

void PuenteH::izquierdaAtras(uint8_t v)
{
    analogWrite(ena, (uint8_t)(v * factorIzq));
    digitalWrite(in1, LOW);
    digitalWrite(in2, HIGH);
}

void PuenteH::detenerIzquierda()
{
    analogWrite(ena, 0);
    digitalWrite(in1, LOW);
    digitalWrite(in2, LOW);
}

void PuenteH::derechaAdelante(uint8_t v)
{
    analogWrite(enb, (uint8_t)(v * factorDer));
    digitalWrite(in3, HIGH);
    digitalWrite(in4, LOW);
}

void PuenteH::derechaAtras(uint8_t v)
{
    analogWrite(enb, (uint8_t)(v * factorDer));
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
    analogWrite(ena, 0);
    analogWrite(enb, 0);
    digitalWrite(in1, LOW);
    digitalWrite(in2, LOW);
    digitalWrite(in3, LOW);
    digitalWrite(in4, LOW);
}