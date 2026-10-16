#include "logica.h"
#include "config.h"

#define DIST_PARED 20.0

// ==================== INICIALIZACIÓN ====================
void pistaA_init(Movimiento &mov)
{
    Serial.println("[PISTA A] Inicializando MAZE...");
    mov.setVelocidad(130);
}

// ==================== LÓGICA PRINCIPAL ====================
bool pistaA_ejecutar(Movimiento &mov, SensorColor &color)
{
    // ============ 1. LEER COLOR ============
    ColorDetectado c = color.detectarColorNuevo();

    if (c == COLOR_A_1 || c == COLOR_A_2 ||
        c == COLOR_A_3 || c == COLOR_A_4)
    {
        Serial.print("[PISTA A] Color detectado: ");
        Serial.println((int)c);
        mostrarColor(c);
        return false;
    }

    if (c == COLOR_A_FINAL)
    {
        Serial.println("[PISTA A] Checkpoint final alcanzado!");
        mov.detener();
        return true;
    }

    // ============ 2. LEER ULTRASÓNICOS ============
    float dFrente = mov.getDistanciaFrontal();
    float dDer = mov.getDistanciaDerecha();
    float dIzq = mov.getDistanciaIzquierda();

    if (dFrente < 0)
        dFrente = 999;
    if (dDer < 0)
        dDer = 999;
    if (dIzq < 0)
        dIzq = 999;

    bool paredFrente = (dFrente < DIST_PARED);
    bool paredDer = (dDer < DIST_PARED);
    bool paredIzq = (dIzq < DIST_PARED);

    // ============ 3. DECIDIR MOVIMIENTO ============
    if (!paredFrente)
    {
        mov.avanzar();
    }
    else if (paredFrente && !paredDer)
    {
        Serial.println("[PISTA A] Giro derecha");
        mov.detener();
        delay(150);
        mov.girarDerecha90();
    }
    else if (paredFrente && paredDer && !paredIzq)
    {
        Serial.println("[PISTA A] Giro izquierda");
        mov.detener();
        delay(150);
        mov.girarIzquierda90();
    }
    else
    {
        Serial.println("[PISTA A] Giro 180");
        mov.detener();
        delay(150);
        mov.girar180();
    }

    return false;
}

// ==================== UTILIDADES ====================
void mostrarColor(ColorDetectado color)
{
    Serial.print("[COLOR] Mostrando: ");
    Serial.println((int)color);
    delay(3000);
}

void apagarLEDs()
{
    // TODO: cuando tengas LEDs, apágalos aquí
}