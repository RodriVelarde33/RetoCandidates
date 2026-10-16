#include "moduloPuenteH.h"

// Constructor: swap porque motores estan invertidos fisicamente
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

// Init pines
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

// Compensacion de motores
void PuenteH::setFactores(float izq, float der)
{
    factorIzq = izq;
    factorDer = der;
}

// Motor izquierdo adelante
void PuenteH::izquierdaAdelante(uint8_t velocidad)
{
    analogWrite(ena, (uint8_t)(velocidad * factorIzq));
    digitalWrite(in1, HIGH);
    digitalWrite(in2, LOW);
}

// Motor izquierdo atras
void PuenteH::izquierdaAtras(uint8_t velocidad)
{
    analogWrite(ena, (uint8_t)(velocidad * factorIzq));
    digitalWrite(in1, LOW);
    digitalWrite(in2, HIGH);
}

// Parar motor izquierdo
void PuenteH::detenerIzquierda()
{
    analogWrite(ena, 0);
    digitalWrite(in1, LOW);
    digitalWrite(in2, LOW);
}

// Motor derecho adelante
void PuenteH::derechaAdelante(uint8_t velocidad)
{
    analogWrite(enb, (uint8_t)(velocidad * factorDer));
    digitalWrite(in3, HIGH);
    digitalWrite(in4, LOW);
}

// Motor derecho atras
void PuenteH::derechaAtras(uint8_t velocidad)
{
    analogWrite(enb, (uint8_t)(velocidad * factorDer));
    digitalWrite(in3, LOW);
    digitalWrite(in4, HIGH);
}

// Parar motor derecho
void PuenteH::detenerDerecha()
{
    analogWrite(enb, 0);
    digitalWrite(in3, LOW);
    digitalWrite(in4, LOW);
}

// Parar todo
void PuenteH::detener()
{
    analogWrite(ena, 0);
    analogWrite(enb, 0);
    digitalWrite(in1, LOW);
    digitalWrite(in2, LOW);
    digitalWrite(in3, LOW);
    digitalWrite(in4, LOW);
}