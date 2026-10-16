#include "movimiento.h"
#include <math.h>
#include "../logica/config.h"

static const bool PUENTE_CON_SWAP = true;

// Aplica velocidad a un lado del puente
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

// Constructor
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

// Init + calibrar giroscopio
bool Movimiento::begin()
{
    Wire.begin(); // I2C
    delay(50);

    puente.begin();
    frontal.begin();
    izquierdo.begin();
    derecho.begin();
    colorFrontal.begin();

    // MPU6050 responde?
    if (!giro.begin())
    {
        Serial.println("[ERROR] MPU6050 no responde por I2C/multiplexor.");
        detener();
        return false;
    }

    // Calibrar offset (robot quieto)
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

// Leer los 3 en secuencia, con promedio en el frontal (sin eco = 999)
void Movimiento::leerDistancias(float &f, float &i, float &d)
{
    // Promedio de 2 lecturas del frontal (filtra ruido)
    float f1 = frontal.getDistanceCm();
    delay(8);
    float f2 = frontal.getDistanceCm();
    f = (f1 + f2) / 2.0f;

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
// Integra velocidad angular en rumbo
void Movimiento::actualizarRumbo()
{
    unsigned long ahora = millis();
    float dt = (ahora - tRumbo) / 1000.0f;
    tRumbo = ahora;

    // Si paso mucho tiempo sin actualizar, descarta (evita saltos)
    if (dt > 0.3f)
        return;

    if (!giro.actualizar())
        return;

    float gz = (giro.getGiroZ() - offsetGiroZ) * SIGNO_GIRO;

    // Deadzone anti-ruido
    if (fabs(gz) < 0.3f)
        gz = 0;

    rumbo += gz * dt;
}

// El rumbo actual es el que quiero mantener
void Movimiento::fijarRumbo()
{
    tRumbo = millis();
    rumboObjetivo = rumbo;
}

// ==================== AVANZAR RECTO ====================
// Recto con rumbo fijo + centrado con paredes
void Movimiento::avanzarConParedes(float dIzq, float dDer)
{
    actualizarRumbo();

    bool hayIzq = (dIzq > 0 && dIzq < DIST_PARED_LATERAL_CM);
    bool hayDer = (dDer > 0 && dDer < DIST_PARED_LATERAL_CM);

    // errLat > 0 -> pegado a la izq -> ir a la derecha
    float errLat = 0;
    if (hayIzq && hayDer)
        errLat = (dDer - dIzq) / 2.0f;
    else if (hayIzq)
        errLat = DIST_LATERAL_IDEAL_CM - dIzq;
    else if (hayDer)
        errLat = dDer - DIST_LATERAL_IDEAL_CM;

    // Las paredes solo inclinan un poco el rumbo
    float ajuste = constrain(KP_PARED_GRADOS * errLat,
                             -MAX_AJUSTE_RUMBO, MAX_AJUSTE_RUMBO);
    float err = (rumboObjetivo + ajuste) - rumbo;

    int corr = (int)constrain(KP_RUMBO * err, -CORR_MAX, CORR_MAX);

    // Limitar correccion a 50% de la velocidad (evita giros bruscos)
    int corrMax = velocidad / 2;
    corr = constrain(corr, -corrMax, corrMax);

    // corr > 0 -> rueda izq más rápida -> gira derecha
    mover(max(0, velocidad + corr), max(0, velocidad - corr));
}

// ==================== GIRO CON CORRECCION ====================
void Movimiento::girarGrados(float grados)
{
    detener();
    delay(100);

    actualizarRumbo();
    float rumboInicial = rumbo;

    // Anticipo para compensar inercia
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

    Serial.print("[GIRO] pedido=");
    Serial.print(grados);
    Serial.print(" | objetivo=");
    Serial.print(objetivo);
    Serial.print(" | rumboIni=");
    Serial.println(rumboInicial);

    // FASE 1: GIRAR
    unsigned long tInicio = millis();
    const unsigned long tMax = 5000;
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

        // err > 0 -> falta girar a la derecha
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
        Serial.println("[AVISO] Giro: no alcanzo objetivo en fase 1, se corrige.");
    }

    // Objetivo real (sin anticipo)
    float objetivoReal = rumboInicial + grados;
    float error = objetivoReal - rumbo;

    Serial.print("[GIRO] real=");
    Serial.print(rumbo - rumboInicial);
    Serial.print(" | error=");
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

    Serial.print("[GIRO] final. Antes=");
    Serial.print(rumboInicial);
    Serial.print(" Despues=");
    Serial.print(rumbo);
    Serial.print(" Delta=");
    Serial.print(rumbo - rumboInicial);
    Serial.print(" | correcciones=");
    Serial.println(intentosCorreccion);

    fijarRumbo();
}

// ==================== GIRO 180 ====================
// Dos veces 90 + correccion intermedia con laterales
void Movimiento::girar180()
{
    girarGrados(90);
    delay(200);

    // Corregir alineacion antes del segundo giro
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