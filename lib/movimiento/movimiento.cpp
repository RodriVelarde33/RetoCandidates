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
    offsetGiroZ = 0;
    rumbo = 0;
    rumboObjetivo = 0;
    tRumbo = 0;
}

bool Movimiento::begin()
{
    puente.begin();
    frontal.begin();
    izquierdo.begin();
    derecho.begin();
    colorFrontal.begin();
    if (!giro.begin())
    {
        Serial.println("[ERROR] MPU6050 no responde por I2C/multiplexor.");
        detener();
        return false;
    }

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

void Movimiento::leerDistancias(float &f, float &i, float &d)
{
    f = frontal.getDistanceCm();
    delay(8);
    d = derecho.getDistanceCm();
    delay(8);
    i = izquierdo.getDistanceCm();

    if (f < 0)
        f = 999;
    if (d < 0)
        d = 999;
    if (i < 0)
        i = 999;
}

// ==================== GIROSCOPIO ====================
void Movimiento::actualizarRumbo()
{
    unsigned long ahora = millis();
    float dt = (ahora - tRumbo) / 1000.0f;
    tRumbo = ahora;

    if (dt > 0.5f)
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

void Movimiento::esperarActualizando(unsigned long ms)
{
    unsigned long t0 = millis();
    while (millis() - t0 < ms)
    {
        actualizarRumbo();
        delay(5);
    }
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

    mover(max(0, velocidad + corr), max(0, velocidad - corr));
}

void Movimiento::avanzarRecto()
{
    avanzarConParedes(999, 999);
}

// ==================== GIRO CON CORRECCION POST-GIRO ====================
void Movimiento::girarGrados(float grados)
{
    detener();
    delay(100);

    actualizarRumbo();
    float rumboInicial = rumbo;

    float anticipo = 0;
    if (fabs(grados) >= 150.0f)
        anticipo = 5.0f;
    else if (fabs(grados) >= 80.0f)
        anticipo = 3.0f;
    else
        anticipo = 1.5f;

    float objetivo;
    if (grados > 0)
        objetivo = rumboInicial + grados - anticipo;
    else
        objetivo = rumboInicial + grados + anticipo;

    Serial.print("[GIRO] pedido: ");
    Serial.print(grados);
    Serial.print(" | objetivo: ");
    Serial.println(objetivo);

    // FASE 1: GIRAR
    unsigned long tInicio = millis();
    const unsigned long tMax = 4000;
    bool objetivoAlcanzado = false;

    while (millis() - tInicio < tMax)
    {
        actualizarRumbo();
        float err = objetivo - rumbo;

        if (fabs(err) <= TOL_GIRO_GRADOS)
        {
            objetivoAlcanzado = true;
            break;
        }

        int v;
        if (fabs(err) < 15.0f)
            v = VEL_GIRO_MIN_PWM;
        else
            v = velocidadGiro;

        // El signo del error indica hacia dónde debe corregirse el rumbo.
        if (err > 0)
            mover(-v, v);
        else
            mover(v, -v);

        delay(5);
    }

    detener();
    delay(400);
    actualizarRumbo();

    if (!objetivoAlcanzado)
    {
        Serial.println("[AVISO] Giro: no se alcanzo el objetivo en la fase inicial; se intentara corregir.");
    }

    float objetivoReal;
    if (grados > 0)
        objetivoReal = rumboInicial + grados;
    else
        objetivoReal = rumboInicial + grados;

    float error = objetivoReal - rumbo;

    Serial.print("[GIRO] real: ");
    Serial.print(rumbo - rumboInicial);
    Serial.print(" | error: ");
    Serial.println(error);

    // FASE 2: CORREGIR
    int intentosCorreccion = 0;
    while (fabs(error) > TOL_GIRO_GRADOS && intentosCorreccion < 5)
    {
        intentosCorreccion++;

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

    Serial.print("[GIRO] final: ");
    Serial.print(rumbo - rumboInicial);
    Serial.print(" | correcciones: ");
    Serial.println(intentosCorreccion);

    fijarRumbo();
}

// ==================== GIRO 180 ====================
void Movimiento::girar180()
{
    girarGrados(90);
    delay(200);
    girarGrados(90);
}
