#include "logica.h"
#include "config.h"

// Umbrales
#define DIST_FRENTE_PARADA_CM 15.0f
#define TIEMPO_AVANCE_MAX_MS 2500
#define COOLDOWN_COLOR_MS 3000
#define TIEMPO_RETROCESO_MS 200 // <-- retroceso antes de girar

// Estado
static unsigned long ultimoMostrarColor = 0;

// Es color intermedio?
static bool esColorIntermedio(ColorDetectado c)
{
    return c == COLOR_A_1 || c == COLOR_A_2 ||
           c == COLOR_A_3 || c == COLOR_A_4;
}

// Pared al frente? (2 de 3 lecturas)
static bool hayParedFrente(Movimiento &mov, float umbral)
{
    int cuantos = 0;
    for (int i = 0; i < 3; i++)
    {
        float d = mov.getDistanciaFrontal();
        if (d > 0 && d < umbral)
            cuantos++;
        delay(15);
    }
    return cuantos >= 2;
}

// Checar color (0=seguir, 1=FIN, 2=color mostrado)
static int checarColor(Movimiento &mov, SensorColor &color)
{
    if (millis() - ultimoMostrarColor < COOLDOWN_COLOR_MS)
        return 0;

    ColorDetectado c = color.detectarColorNuevo();

    if (c == COLOR_A_FINAL)
    {
        Serial.println("[PISTA A] FIN!");
        return 1;
    }

    if (esColorIntermedio(c))
    {
        Serial.print("[PISTA A] Color: ");
        Serial.println((int)c);
        mov.detener();
        mostrarColor(c);
        ultimoMostrarColor = millis();
        return 2;
    }

    return 0;
}

// Init
void pistaA_init(Movimiento &mov)
{
    Serial.println("[PISTA A] Inicializando MAZE...");
    mov.setVelocidad(VEL_AVANCE_PWM);
    mov.setVelocidadGiro(VEL_GIRO_PWM);

    pinMode(LED_1, OUTPUT);
    apagarLEDs();

    ultimoMostrarColor = 0;
}

// Logica principal
bool pistaA_ejecutar(Movimiento &mov, SensorColor &color)
{
    // Chequeo de color
    int r = checarColor(mov, color);
    if (r == 1)
    {
        mov.detener();
        return true;
    }
    if (r == 2)
        return false;

    // Avanzar
    Serial.println("[PISTA A] Avanzando...");
    unsigned long t0 = millis();
    bool paredCerca = false;

    while (millis() - t0 < TIEMPO_AVANCE_MAX_MS)
    {
        // Chequeo de color
        r = checarColor(mov, color);
        if (r == 1)
        {
            mov.detener();
            return true;
        }
        if (r == 2)
            return false;

        // Pared cerca? -> parar
        float dF = mov.getDistanciaFrontal();
        if (dF > 0 && dF < DIST_FRENTE_PARADA_CM)
        {
            delay(30);
            float dF2 = mov.getDistanciaFrontal();
            if (dF2 > 0 && dF2 < DIST_FRENTE_PARADA_CM)
            {
                Serial.print("[PISTA A] Pared cerca (");
                Serial.print(dF2);
                Serial.println(" cm)");
                paredCerca = true;
                break;
            }
        }

        // Avanzar con correccion
        float dI = mov.getDistanciaIzquierda();
        float dD = mov.getDistanciaDerecha();
        mov.avanzarConParedes(dI, dD);

        delay(10);
    }

    mov.detener();
    delay(200);

    // Si hay pared -> retroceder y girar
    if (paredCerca && hayParedFrente(mov, DIST_FRENTE_PARADA_CM))
    {
        Serial.print("[PISTA A] Retroceder ");
        Serial.print(TIEMPO_RETROCESO_MS);
        Serial.println(" ms");
        mov.retrocederTiempo(TIEMPO_RETROCESO_MS);
        delay(200);

        Serial.println("[PISTA A] -> Giro DERECHA");
        mov.girarDerecha90();
    }
    else
    {
        Serial.println("[PISTA A] -> Seguir");
    }

    return false;
}

// Utilidades
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