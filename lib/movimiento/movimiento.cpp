#include "movimiento.h"
#include <math.h>
#include <Wire.h>
#include "../logica/config.h"

static const bool PUENTE_CON_SWAP = true;

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

Movimiento::Movimiento(PuenteH &p, SensorUltrasonico &f, SensorUltrasonico &i, SensorUltrasonico &d,
                       SensorColor &c, Garra &g, SensorGiro &gi)
    : puente(p), frontal(f), izquierdo(i), derecho(d), colorFrontal(c), garra(g), giro(gi)
{
    velocidad = VEL_AVANCE_PWM;
    velocidadGiro = VEL_GIRO_PWM;
    offsetGiroZ = 0.0f;
    rumbo = 0.0f;
    rumboObjetivo = 0.0f;
    tRumbo = millis();
    maniobra = Maniobra::NINGUNA;
    tManiobra = millis();
    finRetroceso = 0;
    objetivoGiro = 0.0f;
    giroEstable = false;
    atasco = false;
    ultimaDistanciaFrontal = -1.0f;
    tUltimoProgreso = millis();
}

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
        Serial.println(F("[ERROR] MPU6050 no responde"));
        detener();
        return false;
    }

    float suma = 0.0f;
    for (int i = 0; i < CALIB_MUESTRAS; i++)
    {
        giro.actualizar();
        suma += giro.getGiroZ();
        delay(CALIB_TIEMPO_MS);
    }
    offsetGiroZ = suma / (float)CALIB_MUESTRAS;
    rumbo = 0.0f;
    rumboObjetivo = 0.0f;
    tRumbo = millis();
    detener();
    return true;
}

void Movimiento::actualizarRumbo()
{
    unsigned long ahora = millis();
    float dt = (ahora - tRumbo) / 1000.0f;
    tRumbo = ahora;

    if (dt <= 0.0f || dt > 0.25f)
        return;
    if (!giro.actualizar())
        return;

    float gz = (giro.getGiroZ() - offsetGiroZ) * SIGNO_GIRO;
    if (fabs(gz) < GYRO_DEADBAND_DPS)
        gz = 0.0f;

    rumbo += gz * dt;
}

void Movimiento::update()
{
    unsigned long ahora = millis();
    frontal.update(ahora);
    izquierdo.update(ahora);
    derecho.update(ahora);

    actualizarRumbo();

    switch (maniobra)
    {
    case Maniobra::AVANZAR:
    {
        float dI = izquierdo.getDistanceCm();
        float dD = derecho.getDistanceCm();
        bool hayIzq = dI > 0.0f && dI < DIST_PARED_LATERAL_CM;
        bool hayDer = dD > 0.0f && dD < DIST_PARED_LATERAL_CM;
        float errLat = 0.0f;

        if (hayIzq && hayDer)
            errLat = (dD - dI) / 2.0f;
        else if (hayDer)
            errLat = dD - DIST_LATERAL_IDEAL_CM;
        else if (hayIzq)
            errLat = DIST_LATERAL_IDEAL_CM - dI;

        float ajuste = constrain(KP_PARED_GRADOS * errLat, -MAX_AJUSTE_RUMBO, MAX_AJUSTE_RUMBO);
        float err = (rumboObjetivo + ajuste) - rumbo;
        int corr = (int)constrain(KP_RUMBO * err, -CORR_MAX, CORR_MAX);
        int corrMax = velocidad / 2;
        corr = constrain(corr, -corrMax, corrMax);

        aplicarMotores(max(0, velocidad + corr), max(0, velocidad - corr));

        float dF = frontal.getDistanceCm();
        if (dF > 0.0f && dF < 8.0f)
        {
            if (ultimaDistanciaFrontal < 0.0f || fabs(dF - ultimaDistanciaFrontal) >= ATASCO_DELTA_MIN_CM)
            {
                ultimaDistanciaFrontal = dF;
                tUltimoProgreso = ahora;
                atasco = false;
            }
            if (ahora - tUltimoProgreso >= ATASCO_TIMEOUT_MS)
            {
                atasco = true;
                detener();
                maniobra = Maniobra::NINGUNA;
            }
        }
        else
        {
            ultimaDistanciaFrontal = dF;
            tUltimoProgreso = ahora;
            atasco = false;
        }
        break;
    }
    case Maniobra::RETROCEDER_TIEMPO:
    {
        aplicarMotores(-velocidad, -velocidad);
        if (ahora >= finRetroceso)
        {
            detener();
            maniobra = Maniobra::NINGUNA;
        }
        break;
    }
    case Maniobra::GIRAR:
    {
        float error = objetivoGiro - rumbo;
        if (fabs(error) <= TOL_GIRO_GRADOS)
        {
            detener();
            maniobra = Maniobra::NINGUNA;
        }
        else
        {
            int pwm = VEL_GIRO_MIN_PWM + (int)constrain(fabs(error) * KP_GIRO, 0.0f, (float)PWM_GIRO_MAX_EXTRA);
            girarConError(error, constrain(pwm, VEL_GIRO_MIN_PWM, velocidadGiro));
        }
        break;
    }
    default:
        break;
    }
}

void Movimiento::aplicarMotores(int vIzq, int vDer)
{
    vIzq = (int)(vIzq * TRIM_REAL_IZQ);
    vDer = (int)(vDer * TRIM_REAL_DER);
    aplicar(puente, !PUENTE_CON_SWAP, constrain(vIzq, -255, 255));
    aplicar(puente, PUENTE_CON_SWAP, constrain(vDer, -255, 255));
}

void Movimiento::girarConError(float error, int pwm)
{
    if (error > 0.0f)
        aplicarMotores(-pwm, pwm);
    else
        aplicarMotores(pwm, -pwm);
}

void Movimiento::detener() { puente.detener(); }
float Movimiento::getDistanciaFrontal() { return frontal.getDistanceCm(); }
float Movimiento::getDistanciaIzquierda() { return izquierdo.getDistanceCm(); }
float Movimiento::getDistanciaDerecha() { return derecho.getDistanceCm(); }

void Movimiento::startForward()
{
    detener();
    rumboObjetivo = rumbo;
    tRumbo = millis();
    ultimaDistanciaFrontal = frontal.getDistanceCm();
    tUltimoProgreso = millis();
    atasco = false;
    maniobra = Maniobra::AVANZAR;
}

void Movimiento::startBackwardMs(unsigned long ms)
{
    detener();
    finRetroceso = millis() + ms;
    maniobra = Maniobra::RETROCEDER_TIEMPO;
}

void Movimiento::startTurn(float grados)
{
    detener();
    actualizarRumbo();
    rumboObjetivo = rumbo;
    objetivoGiro = rumbo + grados;
    giroEstable = false;
    maniobra = Maniobra::GIRAR;
}

bool Movimiento::isManeuverDone() const { return maniobra == Maniobra::NINGUNA; }