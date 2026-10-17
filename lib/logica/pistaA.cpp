#include "logica.h"
#include "config.h"

enum class Estado
{
    AVANZANDO,
    AVANZAR_UN_POCO_MAS,
    RETROCESO,
    GIRANDO_DER,
    GIRANDO_180,
    RECUPERANDO,
    FIN
};

static Estado estado = Estado::AVANZANDO;
static unsigned long ultimoColorAceptado = 0, tLed = 0, tEstado = 0;
static int parpadeosRestantes = 0;
static bool ledEncendido = false;
static bool paredDerConfirmada = false;

static void iniciarParpadeo(ColorDetectado c)
{
    switch (c)
    {
    case ColorDetectado::AZUL:
        parpadeosRestantes = 1;
        break;
    case ColorDetectado::AMARILLO:
        parpadeosRestantes = 2;
        break;
    case ColorDetectado::NARANJA:
        parpadeosRestantes = 3;
        break;
    case ColorDetectado::ROSA:
        parpadeosRestantes = 4;
        break;
    default:
        parpadeosRestantes = 0;
        return;
    }
    ledEncendido = false;
    tLed = millis();
    digitalWrite(LED_1, LOW);
}

void pistaA_init(Movimiento &mov)
{
    mov.setVelocidad(VEL_AVANCE_PWM);
    mov.setVelocidadGiro(VEL_GIRO_PWM);
    pinMode(LED_1, OUTPUT);
    digitalWrite(LED_1, LOW);
    estado = Estado::AVANZANDO;
    ultimoColorAceptado = 0;
    tEstado = millis();
    paredDerConfirmada = false;
    mov.startForward();
}

bool pistaA_ejecutar(Movimiento &mov, SensorColor &color)
{
    mov.update();

    // Manejo del LED
    if (parpadeosRestantes > 0)
    {
        unsigned long ahora = millis();
        if (!ledEncendido && ahora - tLed >= LED_OFF_MS)
        {
            digitalWrite(LED_1, HIGH);
            ledEncendido = true;
            tLed = ahora;
        }
        else if (ledEncendido && ahora - tLed >= LED_ON_MS)
        {
            digitalWrite(LED_1, LOW);
            ledEncendido = false;
            tLed = ahora;
            parpadeosRestantes--;
        }
    }

    unsigned long ahora = millis();
    float dF = mov.getDistanciaFrontal();
    float dD = mov.getDistanciaDerecha();

    switch (estado)
    {
    case Estado::AVANZANDO:
    {
        ColorDetectado c = color.detectarColorEstable();
        if (c == COLOR_A_FINAL)
        {
            mov.detener();
            estado = Estado::FIN;
            return true;
        }
        if (c != ColorDetectado::NINGUNO && c != COLOR_A_FINAL && ahora - ultimoColorAceptado >= COLOR_COOLDOWN_MS)
        {
            mov.detener();
            ultimoColorAceptado = ahora;
            iniciarParpadeo(c);
            break;
        }

        if (mov.isStuck())
        {
            mov.detener();
            mov.startBackwardMs(RETROCESO_ATASCO_MS);
            estado = Estado::RECUPERANDO;
            break;
        }

        bool paredDerAhora = (dD > 0 && dD < DIST_PARED_LATERAL_CM);
        bool paredFrente = (dF > 0 && dF < DIST_FRENTE_PARADA_CM);

        // 1. ¿Pared frontal? -> Si hay pared, retrocede y gira a la derecha o izquierda
        if (paredFrente)
        {
            Serial.println(F("[PISTA] Pared al frente. Retrocediendo."));
            mov.detener();
            mov.startBackwardMs(RETROCESO_PRE_GIRO_MS);
            estado = Estado::RETROCESO;
            // Si la derecha también está tapada, giramos 180 (callejón sin salida)
            if (paredDerAhora)
            {
                estado = Estado::GIRANDO_180; // Saltamos directo al giro de 180
            }
            else
            {
                estado = Estado::GIRANDO_DER; // Giro normal de 90
            }
            break;
        }

        // 2. ¿Hueco a la derecha? (Pared derecha desapareció)
        if (paredDerConfirmada && !paredDerAhora)
        {
            Serial.println(F("[PISTA] Hueco a la derecha. Avanzando un poco más."));
            mov.startForward();
            tEstado = ahora;
            estado = Estado::AVANZAR_UN_POCO_MAS;
            break;
        }
        paredDerConfirmada = paredDerAhora;
        break;
    }

    case Estado::AVANZAR_UN_POCO_MAS:
    {
        // Avanza 300ms (~4cm) para centrarse en el hueco
        if (ahora - tEstado > 300)
        {
            mov.detener();
            mov.startBackwardMs(RETROCESO_PRE_GIRO_MS);
            estado = Estado::RETROCESO;
        }
        break;
    }

    case Estado::RETROCESO:
    {
        if (mov.isManeuverDone())
        {
            // Si el estado anterior era GIRANDO_180, hacemos el giro de 180
            if (estado == Estado::GIRANDO_180)
            {
                mov.startTurn(180.0f);
            }
            else
            {
                mov.startTurn(90.0f);
            }
            // Esto es un truco para no perder el estado, lo manejamos abajo
            // (Mejor lo hacemos directo en el AVANZANDO)
        }
        break;
    }

    case Estado::GIRANDO_DER:
    {
        if (mov.isManeuverDone() || ahora - tEstado > 3000)
        {
            mov.startForward();
            paredDerConfirmada = false;
            estado = Estado::AVANZANDO;
        }
        break;
    }

    case Estado::GIRANDO_180:
    {
        if (mov.isManeuverDone() || ahora - tEstado > 3000)
        {
            mov.startForward();
            paredDerConfirmada = false;
            estado = Estado::AVANZANDO;
        }
        break;
    }

    case Estado::RECUPERANDO:
    {
        if (mov.isManeuverDone())
        {
            mov.startForward();
            estado = Estado::AVANZANDO;
        }
        break;
    }

    case Estado::FIN:
        mov.detener();
        return true;
    }
    return false;
}

void mostrarColor(ColorDetectado color) { iniciarParpadeo(color); }
void apagarLEDs()
{
    digitalWrite(LED_1, LOW);
    parpadeosRestantes = 0;
}