#include "movimiento.h"
#include <math.h>
#include "../logica/config.h"

static const bool PUENTE_CON_SWAP = true;

// Aplica velocidad a un lado
static void aplicar(PuenteH &p, bool ladoLogicoIzq, int v)
{
    if (ladoLogicoIzq)
    {
        if (v >= 0)
            p.izquierdaAdelante(v);
        else
            p.izquierdaAtras(-v);
    }
    else
    {
        if (v >= 0)
            p.derechaAdelante(v);
        else
            p.derechaAtras(-v);
    }
}

Movimiento::Movimiento(PuenteH &puente,
                       SensorUltrasonico &frontal,
                       SensorUltrasonico &izquierdo,
                       SensorUltrasonico &derecho,
                       SensorColor &colorFrontal,
                       Garra &garra,
                       SensorGiro &giro)
    : puente(puente), frontal(frontal), izquierdo(izquierdo),
      derecho(derecho), colorFrontal(colorFrontal), garra(garra), giro(giro)
{
    velocidad = VEL_AVANCE_PWM;
    velocidadGiro = VEL_GIRO_PWM;
    offsetGiroZ = 0;
    rumbo = 0;
    rumboObjetivo = 0;
    tRumbo = 0;
}

// Init
bool Movimiento::begin()
{
    Wire.begin();
    delay(50);

    puente.begin();
    frontal.begin();
    izquierdo.begin();
    derecho.begin();
    colorFrontal.begin();

    if (!giro.begin())
    {
        Serial.println("[ERROR] MPU6050 no responde");
        detener();
        return false;
    }

    // Calibrar offset
    Serial.println("[CALIB] Calibrando giroscopio...");
    float suma = 0;
    for (int i = 0; i < 100; i++)
    {
        giro.actualizar();
        suma += giro.getGiroZ();
        delay(5);
    }
    offsetGiroZ = suma / 100.0f;
    Serial.print("[CALIB] Offset = ");
    Serial.println(offsetGiroZ);

    rumbo = 0;
    rumboObjetivo = 0;
    tRumbo = millis();
    detener();
    return true;
}

// ==================== MOTORES ====================
void Movimiento::mover(int vIzqReal, int vDerReal)
{
    vIzqReal = (int)(vIzqReal * TRIM_REAL_IZQ);
    vDerReal = (int)(vDerReal * TRIM_REAL_DER);
    vIzqReal = constrain(vIzqReal, -255, 255);
    vDerReal = constrain(vDerReal, -255, 255);

    aplicar(puente, !PUENTE_CON_SWAP, vIzqReal);
    aplicar(puente, PUENTE_CON_SWAP, vDerReal);
}

void Movimiento::avanzar() { mover(velocidad, velocidad); }
void Movimiento::retroceder() { mover(-velocidad, -velocidad); }
void Movimiento::detener() { puente.detener(); }

void Movimiento::retrocederTiempo(unsigned long ms)
{
    retroceder();
    delay(ms);
    detener();
}

// ==================== ULTRASONICOS ====================
float Movimiento::getDistanciaFrontal() { return frontal.getDistanceCm(); }
float Movimiento::getDistanciaIzquierda() { return izquierdo.getDistanceCm(); }
float Movimiento::getDistanciaDerecha() { return derecho.getDistanceCm(); }

// ==================== GIROSCOPIO ====================
void Movimiento::actualizarRumbo()
{
    unsigned long ahora = millis();
    float dt = (ahora - tRumbo) / 1000.0f;
    tRumbo = ahora;

    if (dt > 0.3f)
        return;
    if (!giro.actualizar())
        return;

    float gz = (giro.getGiroZ() - offsetGiroZ) * SIGNO_GIRO;
    if (fabs(gz) < 0.3f)
        gz = 0;

    rumbo += gz * dt;
}

void Movimiento::fijarRumbo()
{
    tRumbo = millis();
    rumboObjetivo = rumbo;
}

// ==================== AVANZAR RECTO ====================
void Movimiento::avanzarConParedes(float dIzq, float dDer)
{
    actualizarRumbo();

    bool hayIzq = (dIzq > 0 && dIzq < DIST_PARED_LATERAL_CM);
    bool hayDer = (dDer > 0 && dDer < DIST_PARED_LATERAL_CM);

    float errLat = 0;
    if (hayIzq && hayDer)
        errLat = (dDer - dIzq) / 2.0f;
    else if (hayIzq)
        errLat = DIST_LATERAL_IDEAL_CM - dIzq;
    else if (hayDer)
        errLat = dDer - DIST_LATERAL_IDEAL_CM;

    float ajuste = constrain(KP_PARED_GRADOS * errLat,
                             -MAX_AJUSTE_RUMBO, MAX_AJUSTE_RUMBO);
    float err = (rumboObjetivo + ajuste) - rumbo;

    int corr = (int)constrain(KP_RUMBO * err, -CORR_MAX, CORR_MAX);
    int corrMax = velocidad / 2;
    corr = constrain(corr, -corrMax, corrMax);

    mover(max(0, velocidad + corr), max(0, velocidad - corr));
}

// ==================== GIRO ====================
void Movimiento::girarGrados(float grados)
{
    detener();
    delay(100);

    actualizarRumbo();
    float rumboInicial = rumbo;

    // Anticipo por inercia
    float anticipo;
    if (fabs(grados) >= 150.0f)
        anticipo = 5.0f;
    else if (fabs(grados) >= 80.0f)
        anticipo = 3.0f;
    else
        anticipo = 1.5f;

    float objetivo = (grados > 0)
                         ? rumboInicial + grados - anticipo
                         : rumboInicial + grados + anticipo;

    Serial.print("[GIRO] pedido=");
    Serial.print(grados);
    Serial.print(" | objetivo=");
    Serial.print(objetivo);
    Serial.print(" | ini=");
    Serial.println(rumboInicial);

    // FASE 1: girar
    unsigned long tInicio = millis();
    bool alcanzado = false;

    while (millis() - tInicio < 5000)
    {
        actualizarRumbo();
        float err = objetivo - rumbo;

        if (fabs(err) <= TOL_GIRO_GRADOS)
        {
            alcanzado = true;
            break;
        }

        int v = (fabs(err) < 15.0f) ? VEL_GIRO_MIN_PWM : velocidadGiro;

        if (err > 0)
            mover(-v, v);
        else
            mover(v, -v);

        delay(5);
    }

    detener();
    delay(400);
    actualizarRumbo();

    if (!alcanzado)
        Serial.println("[AVISO] Fase 1 no alcanzo objetivo, se corrige");

    float objetivoReal = rumboInicial + grados;
    float error = objetivoReal - rumbo;

    Serial.print("[GIRO] real=");
    Serial.print(rumbo - rumboInicial);
    Serial.print(" | err=");
    Serial.println(error);

    // FASE 2: corregir
    int intentos = 0;
    while (fabs(error) > TOL_GIRO_GRADOS && intentos < 5)
    {
        intentos++;

        unsigned long tCorr = millis();
        while (millis() - tCorr < 800)
        {
            actualizarRumbo();
            error = objetivoReal - rumbo;

            if (fabs(error) <= TOL_GIRO_GRADOS)
                break;

            int v = VEL_GIRO_MIN_PWM;
            if (fabs(error) > 5.0f)
                v = (VEL_GIRO_MIN_PWM + velocidadGiro) / 2;

            if (error > 0)
                mover(-v, v);
            else
                mover(v, -v);

            delay(5);
        }

        detener();
        delay(400);
        actualizarRumbo();
        error = objetivoReal - rumbo;
    }

    Serial.print("[GIRO] final delta=");
    Serial.print(rumbo - rumboInicial);
    Serial.print(" | corr=");
    Serial.println(intentos);

    fijarRumbo();
}

// Giro 180 = dos veces 90
void Movimiento::girar180()
{
    girarGrados(90);
    delay(200);

    // Corregir alineacion antes del segundo 90
    float dI = getDistanciaIzquierda();
    delay(30);
    float dD = getDistanciaDerecha();
    delay(30);

    if (dI > 0 && dI < DIST_PARED_LATERAL_CM &&
        dD > 0 && dD < DIST_PARED_LATERAL_CM)
    {
        float diff = dI - dD;
        if (fabs(diff) > 2.0f)
        {
            unsigned long tAlinea = millis();
            while (millis() - tAlinea < 500)
            {
                dI = getDistanciaIzquierda();
                delay(20);
                dD = getDistanciaDerecha();
                delay(20);

                if (dI <= 0 || dD <= 0)
                    break;
                diff = dI - dD;
                if (fabs(diff) < 2.0f)
                    break;

                if (diff < 0)
                    mover(velocidadGiro / 2, -velocidadGiro / 2);
                else
                    mover(-velocidadGiro / 2, velocidadGiro / 2);

                delay(30);
            }
            detener();
            delay(100);
        }
    }

    girarGrados(90);
    delay(200);
}