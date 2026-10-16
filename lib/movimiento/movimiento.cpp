#include "movimiento.h"
#include <math.h>
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
    offsetGiroZ = 0.0f;

    rumbo = 0.0f;
    rumboObjetivo = 0.0f;
    tRumbo = millis();

    maniobra = Maniobra::NINGUNA;
    tManiobra = millis();
    finRetroceso = 0;

    objetivoGiro = 0.0f;
    tGiroEstable = 0;
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

    Serial.println(F("[CALIB] Calibrando giroscopio..."));

    float suma = 0.0f;
    for (int i = 0; i < CALIB_MUESTRAS; i++)
    {
        giro.actualizar();
        suma += giro.getGiroZ();
        delay(CALIB_TIEMPO_MS);
    }

    offsetGiroZ = suma / (float)CALIB_MUESTRAS;

    Serial.print(F("[CALIB] Offset Z = "));
    Serial.println(offsetGiroZ, 4);

    rumbo = 0.0f;
    rumboObjetivo = 0.0f;
    tRumbo = millis();

    detener();
    return true;
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

        avanzarConParedes(dI, dD);

        float dF = frontal.getDistanceCm();

        // Solo chequear atasco si hay pared cercana al frente
        bool paredCercana = (dF > 0.0f && dF < 40.0f);

        if (paredCercana)
        {
            if (ultimaDistanciaFrontal < 0.0f ||
                fabs(dF - ultimaDistanciaFrontal) >= ATASCO_DELTA_MIN_CM)
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
            // Sin pared cercana: no puede haber atasco
            ultimaDistanciaFrontal = dF;
            tUltimoProgreso = ahora;
            atasco = false;
        }

        break;
    }

    case Maniobra::RETROCEDER_TIEMPO:
    {
        retroceder();

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
            if (!giroEstable)
            {
                giroEstable = true;
                tGiroEstable = ahora;
            }

            detener();

            if (ahora - tGiroEstable >= GIRO_ESTABLE_MS)
            {
                float errorFinal = objetivoGiro - rumbo;

                if (fabs(errorFinal) > TOL_GIRO_GRADOS)
                {
                    girarConError(errorFinal, VEL_GIRO_MIN_PWM);
                }
                else
                {
                    detener();
                    maniobra = Maniobra::NINGUNA;
                }
            }
        }
        else
        {
            giroEstable = false;

            int pwm = VEL_GIRO_MIN_PWM +
                      (int)constrain(fabs(error) * KP_GIRO,
                                     0.0f,
                                     (float)PWM_GIRO_MAX_EXTRA);

            pwm = constrain(pwm, VEL_GIRO_MIN_PWM, velocidadGiro);

            girarConError(error, pwm);
        }

        break;
    }

    case Maniobra::CENTRAR:
    {
        float dI = izquierdo.getDistanceCm();
        float dD = derecho.getDistanceCm();

        bool validoI = dI > 0.0f && dI < DIST_PARED_LATERAL_CM;
        bool validoD = dD > 0.0f && dD < DIST_PARED_LATERAL_CM;

        if (!validoI || !validoD)
        {
            detener();
            maniobra = Maniobra::NINGUNA;
            break;
        }

        float error = dI - dD;

        if (fabs(error) <= TOL_CENTRADO_CM)
        {
            detener();
            maniobra = Maniobra::NINGUNA;
            break;
        }

        int pwm = constrain((int)fabs(error * KP_CENTRADO),
                            VEL_GIRO_MIN_PWM,
                            VEL_CENTRADO_PWM);

        if (error > 0.0f)
        {
            mover(-pwm, pwm);
        }
        else
        {
            mover(pwm, -pwm);
        }

        break;
    }

    case Maniobra::NINGUNA:
    default:
        break;
    }
}

void Movimiento::aplicarMotores(int vIzqReal, int vDerReal)
{
    vIzqReal = (int)(vIzqReal * TRIM_REAL_IZQ);
    vDerReal = (int)(vDerReal * TRIM_REAL_DER);

    vIzqReal = constrain(vIzqReal, -255, 255);
    vDerReal = constrain(vDerReal, -255, 255);

    aplicar(puente, !PUENTE_CON_SWAP, vIzqReal);
    aplicar(puente, PUENTE_CON_SWAP, vDerReal);
}

void Movimiento::mover(int vIzqReal, int vDerReal)
{
    aplicarMotores(vIzqReal, vDerReal);
}

void Movimiento::avanzar()
{
    aplicarMotores(velocidad, velocidad);
}

void Movimiento::retroceder()
{
    aplicarMotores(-velocidad, -velocidad);
}

void Movimiento::detener()
{
    puente.detener();
}

float Movimiento::getDistanciaFrontal()
{
    return frontal.getDistanceCm();
}

float Movimiento::getDistanciaIzquierda()
{
    return izquierdo.getDistanceCm();
}

float Movimiento::getDistanciaDerecha()
{
    return derecho.getDistanceCm();
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

void Movimiento::fijarRumbo()
{
    tRumbo = millis();
    rumboObjetivo = rumbo;
}

void Movimiento::startForward()
{
    detener();
    fijarRumbo();
    maniobra = Maniobra::AVANZAR;
    tManiobra = millis();

    ultimaDistanciaFrontal = frontal.getDistanceCm();
    tUltimoProgreso = millis();
    atasco = false;
}

void Movimiento::startBackwardMs(unsigned long ms)
{
    detener();
    finRetroceso = millis() + ms;
    maniobra = Maniobra::RETROCEDER_TIEMPO;
    tManiobra = millis();
}

void Movimiento::startTurn(float grados)
{
    detener();
    actualizarRumbo();
    fijarRumbo();

    objetivoGiro = rumbo + grados;
    giroEstable = false;
    tGiroEstable = 0;

    maniobra = Maniobra::GIRAR;
    tManiobra = millis();

    Serial.print(F("[GIRO] Objetivo: "));
    Serial.print(objetivoGiro, 2);
    Serial.print(F(" | Actual: "));
    Serial.println(rumbo, 2);
}

void Movimiento::startCentering()
{
    detener();
    maniobra = Maniobra::CENTRAR;
    tManiobra = millis();
}

bool Movimiento::isManeuverDone() const
{
    return maniobra == Maniobra::NINGUNA;
}

void Movimiento::girarConError(float error, int pwmBase)
{
    if (error > 0.0f)
    {
        mover(-pwmBase, pwmBase);
    }
    else
    {
        mover(pwmBase, -pwmBase);
    }
}

void Movimiento::avanzarConParedes(float dIzq, float dDer)
{
    bool hayIzq = dIzq > 0.0f && dIzq < DIST_PARED_LATERAL_CM;
    bool hayDer = dDer > 0.0f && dDer < DIST_PARED_LATERAL_CM;

    float errLat = 0.0f;

    if (hayIzq && hayDer)
    {
        errLat = (dDer - dIzq) / 2.0f;
    }
    else if (hayIzq)
    {
        errLat = DIST_LATERAL_IDEAL_CM - dIzq;
    }
    else if (hayDer)
    {
        errLat = dDer - DIST_LATERAL_IDEAL_CM;
    }

    float ajuste = constrain(KP_PARED_GRADOS * errLat,
                             -MAX_AJUSTE_RUMBO,
                             MAX_AJUSTE_RUMBO);

    float err = (rumboObjetivo + ajuste) - rumbo;

    int corr = (int)constrain(KP_RUMBO * err, -CORR_MAX, CORR_MAX);
    int corrMax = velocidad / 2;
    corr = constrain(corr, -corrMax, corrMax);

    aplicarMotores(max(0, velocidad + corr), max(0, velocidad - corr));
}

void Movimiento::girarGrados(float grados)
{
    startTurn(grados);
    unsigned long t0 = millis();
    while (!isManeuverDone() && millis() - t0 < GIRO_TIMEOUT_MS)
    {
        update();
    }
    detener();
    fijarRumbo();
}

void Movimiento::girarDerecha90() { girarGrados(90.0f); }
void Movimiento::girarIzquierda90() { girarGrados(-90.0f); }
void Movimiento::girar180() { girarGrados(180.0f); }

void Movimiento::retrocederTiempo(unsigned long ms)
{
    startBackwardMs(ms);
    unsigned long t0 = millis();
    while (!isManeuverDone() && millis() - t0 < ms + 100)
    {
        update();
    }
    detener();
}