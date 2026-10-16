#include "logica.h"
#include "config.h"

enum class EstadoPista
{
    AVANZANDO,
    CENTRANDO,
    CONFIRMANDO_COLOR,
    MOSTRANDO_COLOR,
    RETROCESO_PRE_GIRO,
    GIRANDO,
    RECUPERANDO_ATASCO,
    FIN
};

static EstadoPista estado = EstadoPista::AVANZANDO;

static ColorDetectado colorPendiente = ColorDetectado::NINGUNO;
static unsigned long tEstado = 0;
static unsigned long ultimoColorAceptado = 0;
static unsigned long tLed = 0;
static int parpadeosRestantes = 0;
static bool ledEncendido = false;
static uint8_t reintentosAtasco = 0;

static void cambiarEstado(EstadoPista nuevo)
{
    estado = nuevo;
    tEstado = millis();
}

static bool esColorIntermedio(ColorDetectado c)
{
    return c == COLOR_A_1 || c == COLOR_A_2 ||
           c == COLOR_A_3 || c == COLOR_A_4;
}

static bool paredFrontalConfirmada(Movimiento &mov)
{
    float d1 = mov.getDistanciaFrontal();
    float d2 = mov.getDistanciaFrontal();

    bool p1 = d1 > 0 && d1 < DIST_FRENTE_PARADA_CM;
    bool p2 = d2 > 0 && d2 < DIST_FRENTE_PARADA_CM;

    return p1 && p2;
}

static void iniciarParpadeo(ColorDetectado color)
{
    switch (color)
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

    Serial.print(F("[LED] Color "));
    Serial.print((int)color);
    Serial.print(F(", parpadeos: "));
    Serial.println(parpadeosRestantes);

    ledEncendido = false;
    tLed = millis();
    digitalWrite(LED_1, LOW);
}

static void actualizarLED()
{
    if (parpadeosRestantes <= 0)
        return;

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

void pistaA_init(Movimiento &mov)
{
    mov.setVelocidad(VEL_AVANCE_PWM);
    mov.setVelocidadGiro(VEL_GIRO_PWM);

    pinMode(LED_1, OUTPUT);
    digitalWrite(LED_1, LOW);

    estado = EstadoPista::AVANZANDO;
    tEstado = millis();
    ultimoColorAceptado = 0;
    colorPendiente = ColorDetectado::NINGUNO;
    parpadeosRestantes = 0;
    reintentosAtasco = 0;

    mov.startForward();
}

bool pistaA_ejecutar(Movimiento &mov, SensorColor &color)
{
    mov.update();
    actualizarLED();

    unsigned long ahora = millis();

    switch (estado)
    {

    case EstadoPista::AVANZANDO:
    {
        ColorDetectado c = color.detectarColorEstable();

        if (c == COLOR_A_FINAL)
        {
            mov.detener();
            Serial.println(F("[PISTA A] ROJO detectado. FIN."));
            cambiarEstado(EstadoPista::FIN);
            return true;
        }

        if (esColorIntermedio(c) &&
            ahora - ultimoColorAceptado >= COLOR_COOLDOWN_MS)
        {
            mov.detener();
            colorPendiente = c;
            ultimoColorAceptado = ahora;
            iniciarParpadeo(c);
            cambiarEstado(EstadoPista::MOSTRANDO_COLOR);
            break;
        }

        if (mov.isStuck())
        {
            mov.detener();
            Serial.println(F("[PISTA A] Atasco detectado."));
            reintentosAtasco++;

            if (reintentosAtasco > ATASCO_MAX_REINTENTOS)
            {
                Serial.println(F("[PISTA A] Demasiados atascos. FIN."));
                cambiarEstado(EstadoPista::FIN);
                return true;
            }

            mov.startBackwardMs(RETROCESO_ATASCO_MS);
            cambiarEstado(EstadoPista::RECUPERANDO_ATASCO);
            break;
        }

        if (paredFrontalConfirmada(mov))
        {
            mov.detener();
            Serial.println(F("[PISTA A] Pared frontal. Centrando..."));
            mov.startCentering();
            cambiarEstado(EstadoPista::CENTRANDO);
        }

        break;
    }

    case EstadoPista::CENTRANDO:
    {
        if (mov.isManeuverDone() ||
            ahora - tEstado >= CENTRADO_TIMEOUT_MS)
        {
            mov.detener();
            Serial.println(F("[PISTA A] Centrado terminado."));
            cambiarEstado(EstadoPista::CONFIRMANDO_COLOR);
        }
        break;
    }

    case EstadoPista::CONFIRMANDO_COLOR:
    {
        ColorDetectado c = color.detectarColorEstable();

        if (c == COLOR_A_FINAL)
        {
            mov.detener();
            Serial.println(F("[PISTA A] ROJO tras centrado. FIN."));
            cambiarEstado(EstadoPista::FIN);
            return true;
        }

        if (esColorIntermedio(c) &&
            ahora - ultimoColorAceptado >= COLOR_COOLDOWN_MS)
        {
            colorPendiente = c;
            ultimoColorAceptado = ahora;
            iniciarParpadeo(c);
            cambiarEstado(EstadoPista::MOSTRANDO_COLOR);
            break;
        }

        mov.startBackwardMs(RETROCESO_PRE_GIRO_MS);
        cambiarEstado(EstadoPista::RETROCESO_PRE_GIRO);
        break;
    }

    case EstadoPista::MOSTRANDO_COLOR:
    {
        if (parpadeosRestantes <= 0)
        {
            mov.startBackwardMs(RETROCESO_PRE_GIRO_MS);
            cambiarEstado(EstadoPista::RETROCESO_PRE_GIRO);
        }
        break;
    }

    case EstadoPista::RETROCESO_PRE_GIRO:
    {
        if (mov.isManeuverDone())
        {
            Serial.println(F("[PISTA A] Giro derecha 90 grados."));
            mov.startTurn(90.0f);
            cambiarEstado(EstadoPista::GIRANDO);
        }
        break;
    }

    case EstadoPista::GIRANDO:
    {
        if (mov.isManeuverDone())
        {
            mov.fijarRumbo();
            Serial.println(F("[PISTA A] Giro completado. Avanzando."));
            mov.startForward();
            cambiarEstado(EstadoPista::AVANZANDO);
        }
        else if (ahora - tEstado >= GIRO_TIMEOUT_MS + 500)
        {
            mov.detener();
            mov.fijarRumbo();
            mov.startForward();
            cambiarEstado(EstadoPista::AVANZANDO);
        }
        break;
    }

    case EstadoPista::RECUPERANDO_ATASCO:
    {
        if (mov.isManeuverDone())
        {
            Serial.println(F("[PISTA A] Reintentando avance."));
            mov.startForward();
            cambiarEstado(EstadoPista::AVANZANDO);
        }
        break;
    }

    case EstadoPista::FIN:
        mov.detener();
        return true;
    }

    return false;
}

void mostrarColor(ColorDetectado color)
{
    iniciarParpadeo(color);

    while (parpadeosRestantes > 0)
    {
        actualizarLED();
    }
}

void apagarLEDs()
{
    digitalWrite(LED_1, LOW);
    parpadeosRestantes = 0;
}