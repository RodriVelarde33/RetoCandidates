#include <Arduino.h>

// Pines motor izquierdo
const uint8_t ENA = 2;
const uint8_t IN1 = 8;
const uint8_t IN2 = 9;

// Pines motor derecho
const uint8_t ENB = 3;
const uint8_t IN3 = 10;
const uint8_t IN4 = 11;

// Velocidad de los motores (0 - 255)
const uint8_t velocidad = 150;

// Declaración de funciones
void avanzar();
void retroceder();
void detener();
void derecha();
void izquierda();


void setup()
{
    // Configurar pines del motor izquierdo
    pinMode(ENA, OUTPUT);
    pinMode(IN1, OUTPUT);
    pinMode(IN2, OUTPUT);

    // Configurar pines del motor derecho
    pinMode(ENB, OUTPUT);
    pinMode(IN3, OUTPUT);
    pinMode(IN4, OUTPUT);

    // Empezar con los motores detenidos
    detener();
}


void loop()
{
    
    // Detenerse durante 3 segundos
    detener();
    delay(2000);

    derecha();
    delay(2000);

    izquierda();
    delay(2000);


}


// Hace que ambos lados avancen
void avanzar()
{
    // Establecer velocidad
    analogWrite(ENA, velocidad);
    analogWrite(ENB, velocidad);

    // Motor izquierdo hacia adelante
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);

    // Motor derecho hacia adelante
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
}

void derecha()
{
    // Establecer velocidad
    analogWrite(ENA, 0);
    analogWrite(ENB, velocidad);

    // Motor derecho hacia adelante
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
}

void izquierda()
{
    // Establecer velocidad
    analogWrite(ENA, velocidad);
    analogWrite(ENB, 0);

    // Motor izquierdo hacia adelante
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);

}

// Hace que ambos lados retrocedan
void retroceder()
{
    // Establecer velocidad
    analogWrite(ENA, velocidad);
    analogWrite(ENB, velocidad);

    // Motor izquierdo hacia atras
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);

    // Motor derecho hacia atras
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
}


// Detiene ambos lados
void detener()
{
    // Velocidad 0
    analogWrite(ENA, 0);
    analogWrite(ENB, 0);

    // Desactivar señales de direccion
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);

    digitalWrite(IN3, LOW);
    digitalWrite(IN4, LOW);
}