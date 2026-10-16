#include "logica.h"
#include "config.h"

// ==================== UMBRALES ====================
#define DIST_FRENTE_PARADA_CM 15.0f
#define TIEMPO_AVANCE_MAX_MS 2500
#define COOLDOWN_COLOR_MS 3000

// ==================== ESTADO ====================
static unsigned long ultimoMostrarColor = 0;

// ==================== UTILIDADES ====================
static bool esColorIntermedio(ColorDetectado c)
{
    return c == COLOR_A_1 || c == COLOR_A_2 ||
           c == COLOR_A_3 || c == COLOR_A_4;
}

static bool hayParedFrente(Movimiento &mov, float umbral)
{
    int cuantos = 0;
    for (int i = 0; i < 3; i++)
    {
        float d = mov.getDistanciaFrontal();
        if (d > 0 && d < umbral)
            cuantos++;
        delay(20);
    }
    return cuantos >= 2;
}

// ==================== INICIALIZACION ====================
void pistaA_init(Movimiento &mov)
{
    Serial.println("[PISTA A] Inicializando MAZE...");
    mov.setVelocidad(VEL_AVANCE_PWM);
    mov.setVelocidadGiro(VEL_GIRO_PWM);

    pinMode(LED_1, OUTPUT);
    apagarLEDs();

    ultimoMostrarColor = 0;
}

// ==================== LOGICA PRINCIPAL ====================
bool pistaA_ejecutar(Movimiento &mov, SensorColor &color)
{
    // ============ 1. LEER COLOR (con cooldown) ============
    if (millis() - ultimoMostrarColor >= COOLDOWN_COLOR_MS)
    {
        ColorDetectado c = color.detectarColorNuevo();

        if (c == COLOR_A_FINAL)
        {
            Serial.println("[PISTA A] FIN!");
            mov.detener();
            return true;
        }

        if (esColorIntermedio(c))
        {
            Serial.print("[PISTA A] Color: ");
            Serial.println((int)c);
            mov.detener();
            mostrarColor(c);
            ultimoMostrarColor = millis();
            return false;
        }
    }

    // ============ 2. AVANZAR ============
    Serial.println("[PISTA A] Avanzando...");
    unsigned long t0 = millis();

    while (millis() - t0 < TIEMPO_AVANCE_MAX_MS)
    {
        if (millis() - ultimoMostrarColor >= COOLDOWN_COLOR_MS)
        {
            ColorDetectado c_now = color.detectarColorNuevo();
            if (c_now == COLOR_A_FINAL)
            {
                Serial.println("[PISTA A] FIN (durante avance)!");
                mov.detener();
                return true;
            }
            if (esColorIntermedio(c_now))
            {
                Serial.print("[PISTA A] Color durante avance: ");
                Serial.println((int)c_now);
                mov.detener();
                mostrarColor(c_now);
                ultimoMostrarColor = millis();
                return false;
            }
        }

        if (hayParedFrente(mov, DIST_FRENTE_PARADA_CM))
        {
            Serial.println("[PISTA A] Pared al frente detectada");
            break;
        }

        float dI = mov.getDistanciaIzquierda();
        float dD = mov.getDistanciaDerecha();
        mov.avanzarConParedes(dI, dD);

        delay(10);
    }

    mov.detener();
    delay(300);

    // ============ 3. DECIDIR ============
    if (hayParedFrente(mov, DIST_FRENTE_PARADA_CM))
    {
        Serial.println("[PISTA A] -> Giro DERECHA");
        mov.girarDerecha90();
    }
    else
    {
        Serial.println("[PISTA A] -> Seguir");
    }

    return false;
}

// ==================== UTILIDADES ====================
void mostrarColor(ColorDetectado color)
{
    int parpadeos = 0;

    switch (color)
    {
    case ColorDetectado::AZUL:
        parpadeos = 1;
        break;
    case ColorDetectado::AMARILLO:
        parpadeos = 2;
        break;
    case ColorDetectado::NARANJA:
        parpadeos = 3;
        break;
    case ColorDetectado::ROSA:
        parpadeos = 4;
        break;
    default:
        return;
    }

    Serial.print("[LED] Parpadeos: ");
    Serial.println(parpadeos);

    for (int i = 0; i < parpadeos; i++)
    {
        digitalWrite(LED_1, HIGH);
        delay(LED_ON_MS);
        digitalWrite(LED_1, LOW);
        delay(LED_OFF_MS);
    }

    digitalWrite(LED_1, LOW);
}

void apagarLEDs()
{
    digitalWrite(LED_1, LOW);
}
