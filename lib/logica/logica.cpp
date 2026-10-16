#include "logica.h"
#include "config.h"

#define DIST_PARED 20.0

// Init pista A
void pistaA_init(Movimiento &mov)
{
    Serial.println("[PISTA A] Inicializando MAZE...");
    mov.setVelocidad(130);
}

// Logica principal
bool pistaA_ejecutar(Movimiento &mov, SensorColor &color)
{
    // Leer color
    ColorDetectado c = color.detectarColorNuevo();

    // Color intermedio -> mostrar
    if (c == COLOR_A_1 || c == COLOR_A_2 ||
        c == COLOR_A_3 || c == COLOR_A_4)
    {
        Serial.print("[PISTA A] Color detectado: ");
        Serial.println((int)c);
        mostrarColor(c);
        return false;
    }

    // Rojo -> fin
    if (c == COLOR_A_FINAL)
    {
        Serial.println("[PISTA A] Checkpoint final alcanzado!");
        mov.detener();
        return true;
    }

    // Leer ultrasonicos
    float dFrente = mov.getDistanciaFrontal();
    float dDer = mov.getDistanciaDerecha();
    float dIzq = mov.getDistanciaIzquierda();

    // Sin eco = 999 (libre)
    if (dFrente < 0)
        dFrente = 999;
    if (dDer < 0)
        dDer = 999;
    if (dIzq < 0)
        dIzq = 999;

    bool paredFrente = (dFrente < DIST_PARED);
    bool paredDer = (dDer < DIST_PARED);
    bool paredIzq = (dIzq < DIST_PARED);

    // Decidir
    if (!paredFrente)
    {
        mov.avanzar();
    }
    else if (paredFrente && !paredDer)
    {
        // Pared al frente, derecha libre -> girar derecha
        Serial.println("[PISTA A] Giro derecha");
        mov.detener();
        delay(150);
        mov.girarDerecha90();
    }
    else if (paredFrente && paredDer && !paredIzq)
    {
        // Frente y derecha tapados -> girar izquierda
        Serial.println("[PISTA A] Giro izquierda");
        mov.detener();
        delay(150);
        mov.girarIzquierda90();
    }
    else
    {
        // Todo tapado -> 180
        Serial.println("[PISTA A] Giro 180");
        mov.detener();
        delay(150);
        mov.girar180();
    }

    return false;
}

// Mostrar color
void mostrarColor(ColorDetectado color)
{
    Serial.print("[COLOR] Mostrando: ");
    Serial.println((int)color);
    delay(3000);
}

// Apagar LEDs
void apagarLEDs()
{
    // TODO: cuando tengas LEDs
}