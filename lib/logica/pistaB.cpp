#include "logica.h"
#include "config.h"

enum class EstadoB
{
    BUSCANDO_HUECO,
    FIN
};

static EstadoB estado = EstadoB::BUSCANDO_HUECO;
static unsigned long tEstado = 0;
static unsigned long tImpresion = 0;

static void cambiarEstado(EstadoB nuevo)
{
    estado = nuevo;
    tEstado = millis();
}

void pistaB_init(Movimiento &mov, Garra &garra, uint8_t seccionInicial)
{
    mov.setVelocidad(VEL_S1_PWM);
    mov.setVelocidadGiro(VEL_GIRO_PWM);
    garra.cerrar();

    cambiarEstado(EstadoB::BUSCANDO_HUECO);
    mov.startForward();
}

bool pistaB_ejecutar(Movimiento &mov, SensorColor &colorFrontal, Garra &garra)
{
    mov.update();
    unsigned long ahora = millis();

    switch (estado)
    {
    case EstadoB::BUSCANDO_HUECO:
        // Por ahora solo imprime lo que ven los ultrasónicos
        if (ahora - tImpresion >= 200)
        {
            tImpresion = ahora;
            Serial.print(F("Izq: "));
            Serial.print(mov.getDistanciaIzquierda());
            Serial.print(F("  Der: "));
            Serial.println(mov.getDistanciaDerecha());
        }
        break;

    case EstadoB::FIN:
        mov.detener();
        return true;
    }

    return false;
}