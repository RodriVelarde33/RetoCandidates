#include <Arduino.h>
#include <Wire.h>
#include "logica.h"
#include "config.h"
#include "movimiento.h"
#include "garra.h"
#include "sensorColor.h"
#include "sensorGiro.h"
#include "sensorUltrasonico.h"
#include "moduloPuenteH.h"

// Pines
#define PIN_IN1 8
#define PIN_IN2 9
#define PIN_IN3 10
#define PIN_IN4 11
#define PIN_ENA 2
#define PIN_ENB 3

#define PIN_TRIG_FRONTAL 34
#define PIN_ECHO_FRONTAL 35
#define PIN_TRIG_IZQ 32
#define PIN_ECHO_IZQ 33
#define PIN_TRIG_DER 53
#define PIN_ECHO_DER 52

#define CANAL_COLOR_ABAJO 3
#define PIN_SERVO_GARRA 7

// Instancias
PuenteH puente(PIN_IN1, PIN_IN2, PIN_ENA, PIN_IN3, PIN_IN4, PIN_ENB);
SensorUltrasonico ultrasonicoFrontal(PIN_TRIG_FRONTAL, PIN_ECHO_FRONTAL);
SensorUltrasonico ultrasonicoIzq(PIN_TRIG_IZQ, PIN_ECHO_IZQ);
SensorUltrasonico ultrasonicoDer(PIN_TRIG_DER, PIN_ECHO_DER);
SensorColor colorAbajo(CANAL_COLOR_ABAJO);
SensorGiro giroscopio(0x70, 2, 0x68);
Garra garra(PIN_SERVO_GARRA);

Movimiento movimiento(puente,
                      ultrasonicoFrontal,
                      ultrasonicoIzq,
                      ultrasonicoDer,
                      colorAbajo,
                      garra,
                      giroscopio);

// Estados
enum EstadoGlobal
{
    CALIBRACION,
    EJECUTANDO_PISTA_A,
    FIN_RONDA
};

EstadoGlobal estadoGlobal = CALIBRACION;
unsigned long tiempoInicioRonda = 0;
const unsigned long TIEMPO_CALIBRACION = 3000;
const unsigned long TIEMPO_RONDA = 480000;

// Setup
void setup()
{
    Serial.begin(115200);
    delay(500);
    Serial.println("=== ROBOT CANDIDATES 2026 ===");

    Wire.begin();
    puente.setFactores(1.0, 1.0);

    Serial.println("Init movimiento...");
    bool movimientoOK = movimiento.begin();

    Serial.println("Init garra...");
    garra.inicializar(0, 90);

    tiempoInicioRonda = millis();

    if (!movimientoOK)
    {
        estadoGlobal = FIN_RONDA;
        Serial.println("[ERROR] MPU6050 no responde. Revisa I2C.");
        movimiento.detener();
        return;
    }

    Serial.println("Listo. Iniciando Pista A...");
}

// Loop
void loop()
{
    // Timeout de ronda
    if (estadoGlobal == EJECUTANDO_PISTA_A)
    {
        if (millis() - tiempoInicioRonda >= TIEMPO_RONDA)
        {
            estadoGlobal = FIN_RONDA;
            Serial.println("Tiempo agotado.");
        }
    }

    switch (estadoGlobal)
    {
    case CALIBRACION:
        if (millis() - tiempoInicioRonda >= TIEMPO_CALIBRACION)
        {
            pistaA_init(movimiento);
            tiempoInicioRonda = millis();
            estadoGlobal = EJECUTANDO_PISTA_A;
            Serial.println(">>> EJECUTANDO PISTA A <<<");
        }
        break;

    case EJECUTANDO_PISTA_A:
        if (pistaA_ejecutar(movimiento, colorAbajo))
        {
            estadoGlobal = FIN_RONDA;
            Serial.println(">>> FIN DE PISTA A <<<");
        }
        break;

    case FIN_RONDA:
        movimiento.detener();
        break;
    }
}