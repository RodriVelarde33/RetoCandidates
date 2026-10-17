#include "logica.h"
#include "config.h"

// =====================================================================
// PISTA B - Seccion 1: anillo 3x3 alrededor de la celda central
//
// Misma estructura que pistaA.cpp: un solo enum, un solo switch y una
// sola llamada a mov.update() por loop. Nada de delay().
//
// RESTRICCION MECANICA IMPORTANTE:
//   - El sensor ultrasonico frontal va montado en la garra. Con la garra
//     abierta NO sirve, asi que TODO el recorrido y el acercamiento se
//     hacen con la garra CERRADA.
//   - El sensor de color va al frente de la garra. Se usa para confirmar
//     la precision de la entrada a la celda: solo se abre la garra cuando
//     ve el NARANJA de la pelota.
//
//   Secuencia: hueco -> media unidad -> giro -> acercar CERRADA ->
//   confirmar naranja (con pulsos cortos) -> abrir -> avanzar a ciegas ->
//   cerrar -> retroceder -> salir al checkpoint 1.
// =====================================================================

static const float GRADOS_LADO = 90.0f;

enum class EstadoB
{
    B1_AVANZANDO,
    B1_ESQUINA,
    B1_CENTRANDO,
    B1_GIRANDO_ENTRADA,
    B1_ACERCANDO,     // garra cerrada: ultrasonico frontal operativo
    B1_CONFIRMANDO,   // detenido, esperando lectura de color estable
    B1_ALINEANDO,     // pulso corto hacia adelante, garra cerrada
    B1_ABRIENDO,      // se abre la garra: el ultrasonico deja de servir
    B1_AGARRE,        // avance a ciegas, solo por tiempo
    B1_CERRANDO,
    B1_SALIENDO_CELDA,
    B1_GIRANDO_SALIDA,
    B1_BUSCANDO_SALIDA,
    B1_ESQUINA_SALIDA,
    B1_GIRANDO_HACIA_SALIDA,
    B1_CRUZANDO,
    B1_RECUPERANDO,
    B1_FIN
};

static EstadoB estado = EstadoB::B1_AVANZANDO;
static EstadoB estadoRetorno = EstadoB::B1_AVANZANDO;

static unsigned long tEstado = 0;
static unsigned long tCandidato = 0; // timer de la apertura lateral
static unsigned long tFrente = 0;    // timer de la pared frontal
static unsigned long tArmado = 0;    // timer para armar la busqueda
static unsigned long tInicioSeccion = 0;
static unsigned long tInicioAvance = 0;

static bool armado = false;
static bool pelotaEnGarra = false;
static uint8_t reintentos = 0;
static uint8_t pulsos = 0;
static uint8_t seccion = 1;

static void cambiarEstado(EstadoB nuevo)
{
    estado = nuevo;
    tEstado = millis();
    tCandidato = 0;
    tFrente = 0;
}

// A VEL_B_PWM (calibracion normal de avance)
static unsigned long msPorCm(float cm)
{
    return (unsigned long)(cm * (float)B_MS_POR_CM);
}

// A VEL_B_AGARRE_PWM: el avance lento recorre menos por ms
static unsigned long msPorCmLento(float cm)
{
    return (unsigned long)(cm * (float)B_MS_POR_CM_LENTO);
}

static bool hayPared(float d)
{
    return d > 0.0f && d < B_PARED_LATERAL_CM;
}

static bool hayApertura(float d)
{
    return d <= 0.0f || d > B_APERTURA_CM;
}

static float distanciaLado(Movimiento &mov, int lado)
{
    return (lado > 0) ? mov.getDistanciaDerecha() : mov.getDistanciaIzquierda();
}

// True si la condicion lleva sostenida "ms" milisegundos seguidos.
static bool sostenido(bool condicion, unsigned long ahora,
                      unsigned long &t0, unsigned long ms)
{
    if (!condicion)
    {
        t0 = 0;
        return false;
    }

    if (t0 == 0)
    {
        t0 = ahora;
        return false;
    }

    return (ahora - t0 >= ms);
}

static bool paredFrontal(Movimiento &mov)
{
    float d = mov.getDistanciaFrontal();
    return d > 0.0f && d < B_PARED_FRENTE_CM;
}

static void recuperarAtasco(Movimiento &mov, EstadoB retorno)
{
    mov.detener();
    Serial.println(F("[PISTA B] Atasco. Retrocedo y reintento."));

    estadoRetorno = retorno;
    armado = false;
    tArmado = 0;

    mov.startBackwardMs(RETROCESO_ATASCO_MS);
    cambiarEstado(EstadoB::B1_RECUPERANDO);
}

static void terminarGiro(Movimiento &mov)
{
    mov.detener();
    mov.fijarRumbo();
}

// El ultrasonico solo es confiable con la garra cerrada. Solo se usa en
// los estados previos a la apertura.
static bool proximoAPelota(Movimiento &mov)
{
    float d = mov.getDistanciaFrontal();
    return d > 0.0f && d < B_DIST_PELOTA_CM;
}

// ---------------------------------------------------------------- seccion 1

static bool ejecutarSeccion1(Movimiento &mov, SensorColor &colorFrontal,
                             Garra &garra)
{
    unsigned long ahora = millis();

    if (estado != EstadoB::B1_FIN &&
        ahora - tInicioSeccion >= B_TIMEOUT_S1_MS)
    {
        mov.detener();
        Serial.println(F("[PISTA B] Timeout de la seccion 1."));
        cambiarEstado(EstadoB::B1_FIN);
    }

    switch (estado)
    {

    // Recorre el anillo con la garra cerrada. Si aparece pared al frente es
    // una esquina: gira hacia el lado de la celda. Si el lado de la celda se
    // abre, esa es la unica entrada: entramos.
    case EstadoB::B1_AVANZANDO:
    {
        if (mov.isStuck())
        {
            recuperarAtasco(mov, EstadoB::B1_AVANZANDO);
            break;
        }

        if (sostenido(paredFrontal(mov), ahora, tFrente, B_FRENTE_CONFIRMA_MS))
        {
            mov.detener();
            Serial.println(F("[PISTA B] Esquina. Giro hacia la celda."));
            mov.startTurn(B_LADO_PELOTA * GRADOS_LADO);
            cambiarEstado(EstadoB::B1_ESQUINA);
            break;
        }

        if (!armado)
        {
            // Solo buscamos cuando ya estamos dentro del anillo, con pared
            // a los dos lados. Si no, la casilla de inicio dispara.
            armado = sostenido(hayPared(distanciaLado(mov, +1)) &&
                                   hayPared(distanciaLado(mov, -1)),
                               ahora, tArmado, B_APERTURA_MS);
            break;
        }

        if (sostenido(hayApertura(distanciaLado(mov, B_LADO_PELOTA)) &&
                          hayPared(distanciaLado(mov, -B_LADO_PELOTA)),
                      ahora, tCandidato, B_APERTURA_MS))
        {
            mov.detener();
            Serial.println(F("[PISTA B] Hueco de la celda. Entrando."));
            mov.startForward();
            cambiarEstado(EstadoB::B1_CENTRANDO);
        }
        break;
    }

    case EstadoB::B1_ESQUINA:
    {
        if (mov.isManeuverDone() ||
            ahora - tEstado >= GIRO_TIMEOUT_MS + 500)
        {
            terminarGiro(mov);
            reintentos = 0;
            mov.startForward();
            cambiarEstado(EstadoB::B1_AVANZANDO);
        }
        break;
    }

    // Avanza lo necesario para quedar centrado frente al hueco
    case EstadoB::B1_CENTRANDO:
    {
        if (ahora - tEstado >= msPorCm(B_CENTRAR_CM))
        {
            mov.detener();
            mov.startTurn(B_LADO_PELOTA * GRADOS_LADO);
            cambiarEstado(EstadoB::B1_GIRANDO_ENTRADA);
        }
        break;
    }

    // Ya mirando al centro de la celda: arranca el avance lento CON LA
    // GARRA CERRADA, para que el ultrasonico frontal siga midiendo.
    case EstadoB::B1_GIRANDO_ENTRADA:
    {
        if (mov.isManeuverDone() ||
            ahora - tEstado >= GIRO_TIMEOUT_MS + 500)
        {
            terminarGiro(mov);
            pulsos = 0;
            mov.setVelocidad(VEL_B_AGARRE_PWM);
            mov.mover(VEL_B_AGARRE_PWM, VEL_B_AGARRE_PWM);
            tInicioAvance = ahora;
            cambiarEstado(EstadoB::B1_ACERCANDO);
        }
        break;
    }

    // Acercamiento con la garra cerrada hasta que el ultrasonico ve la
    // pelota, o hasta el limite de la celda por tiempo.
    case EstadoB::B1_ACERCANDO:
    {
        if (proximoAPelota(mov) ||
            ahora - tInicioAvance >= msPorCmLento(B_ENTRADA_CM))
        {
            mov.detener();
            cambiarEstado(EstadoB::B1_CONFIRMANDO);
        }
        break;
    }

    // Confirmacion de precision: el sensor de color va al frente de la
    // garra, asi que tiene que ver el NARANJA de la pelota antes de abrir.
    case EstadoB::B1_CONFIRMANDO:
    {
        ColorDetectado c = colorFrontal.detectarColorEstable();

        if (ahora - tEstado < B_ESPERA_COLOR_MS)
            break;

        if (c == COLOR_B_PELOTA)
        {
            Serial.println(F("[PISTA B] Naranja confirmado. Abro garra."));
            pulsos = 0;
            garra.abrir();
            cambiarEstado(EstadoB::B1_ABRIENDO);
            break;
        }

        if (pulsos < B_PULSOS_ALINEAR)
        {
            // Todavia no la ve: pulso corto y vuelvo a comprobar
            pulsos++;
            mov.setVelocidad(VEL_B_AGARRE_PWM);
            mov.mover(VEL_B_AGARRE_PWM, VEL_B_AGARRE_PWM);
            cambiarEstado(EstadoB::B1_ALINEANDO);
            break;
        }

        Serial.println(F("[PISTA B] Sin naranja tras los pulsos. Intento a ciegas."));
        garra.abrir();
        cambiarEstado(EstadoB::B1_ABRIENDO);
        break;
    }

    case EstadoB::B1_ALINEANDO:
    {
        if (ahora - tEstado >= msPorCmLento(B_PULSO_ALINEAR_CM))
        {
            mov.detener();
            cambiarEstado(EstadoB::B1_CONFIRMANDO);
        }
        break;
    }

    // A partir de aqui la garra esta abierta: el ultrasonico frontal ya no
    // es confiable, todo el agarre es por tiempo.
    case EstadoB::B1_ABRIENDO:
    {
        if (ahora - tEstado >= GARRA_MOVER_MS)
        {
            mov.setVelocidad(VEL_B_AGARRE_PWM);
            mov.mover(VEL_B_AGARRE_PWM, VEL_B_AGARRE_PWM);
            cambiarEstado(EstadoB::B1_AGARRE);
        }
        break;
    }

    case EstadoB::B1_AGARRE:
    {
        if (ahora - tEstado >= msPorCmLento(B_AVANCE_AGARRE_CM))
        {
            mov.detener();
            garra.cerrarSinEspera();
            Serial.println(F("[PISTA B] Garra cerrada sobre la pelota."));
            cambiarEstado(EstadoB::B1_CERRANDO);
        }
        break;
    }

    case EstadoB::B1_CERRANDO:
    {
        if (ahora - tEstado >= GARRA_MOVER_MS)
        {
            // Con la garra cerrada el sensor de color vuelve a servir como
            // chequeo suave de posesion. Solo se reporta, no bloquea.
            pelotaEnGarra = (colorFrontal.detectarColorEstable() == COLOR_B_PELOTA);
            Serial.print(F("[PISTA B] Pelota en garra: "));
            Serial.println(pelotaEnGarra ? F("si") : F("no confirmado"));

            mov.setVelocidad(VEL_B_PWM);
            mov.startBackwardMs(msPorCm(B_RETROCESO_CM));
            cambiarEstado(EstadoB::B1_SALIENDO_CELDA);
        }
        break;
    }

    case EstadoB::B1_SALIENDO_CELDA:
    {
        if (mov.isManeuverDone())
        {
            // Volvemos a mirar a lo largo del anillo
            mov.startTurn(-B_LADO_PELOTA * GRADOS_LADO);
            cambiarEstado(EstadoB::B1_GIRANDO_SALIDA);
        }
        break;
    }

    case EstadoB::B1_GIRANDO_SALIDA:
    {
        if (mov.isManeuverDone() ||
            ahora - tEstado >= GIRO_TIMEOUT_MS + 500)
        {
            terminarGiro(mov);
            armado = false;
            tArmado = 0;
            mov.startForward();
            cambiarEstado(EstadoB::B1_BUSCANDO_SALIDA);
        }
        break;
    }

    // Sigue el anillo hasta la apertura del lado contrario (salida)
    case EstadoB::B1_BUSCANDO_SALIDA:
    {
        if (mov.isStuck())
        {
            recuperarAtasco(mov, EstadoB::B1_BUSCANDO_SALIDA);
            break;
        }

        if (sostenido(paredFrontal(mov), ahora, tFrente, B_FRENTE_CONFIRMA_MS))
        {
            mov.detener();
            mov.startTurn(B_LADO_PELOTA * GRADOS_LADO);
            cambiarEstado(EstadoB::B1_ESQUINA_SALIDA);
            break;
        }

        if (sostenido(hayApertura(distanciaLado(mov, -B_LADO_PELOTA)) &&
                          hayPared(distanciaLado(mov, B_LADO_PELOTA)),
                      ahora, tCandidato, B_APERTURA_MS))
        {
            mov.detener();
            Serial.println(F("[PISTA B] Salida del anillo."));
            mov.startTurn(-B_LADO_PELOTA * GRADOS_LADO);
            cambiarEstado(EstadoB::B1_GIRANDO_HACIA_SALIDA);
        }
        break;
    }

    case EstadoB::B1_ESQUINA_SALIDA:
    {
        if (mov.isManeuverDone() ||
            ahora - tEstado >= GIRO_TIMEOUT_MS + 500)
        {
            terminarGiro(mov);
            reintentos = 0;
            mov.startForward();
            cambiarEstado(EstadoB::B1_BUSCANDO_SALIDA);
        }
        break;
    }

    case EstadoB::B1_GIRANDO_HACIA_SALIDA:
    {
        if (mov.isManeuverDone() ||
            ahora - tEstado >= GIRO_TIMEOUT_MS + 500)
        {
            terminarGiro(mov);
            mov.startForward();
            tInicioAvance = ahora;
            cambiarEstado(EstadoB::B1_CRUZANDO);
        }
        break;
    }

    // Cruza la salida hasta el checkpoint 1 (tile rojo)
    case EstadoB::B1_CRUZANDO:
    {
        if (colorFrontal.detectarColorEstable() == COLOR_B_CHECKPOINT)
        {
            mov.detener();
            Serial.print(F("[PISTA B] Checkpoint 1. Pelota: "));
            Serial.println(pelotaEnGarra ? F("si") : F("no"));
            cambiarEstado(EstadoB::B1_FIN);
            break;
        }

        if (mov.isStuck())
        {
            recuperarAtasco(mov, EstadoB::B1_CRUZANDO);
            break;
        }

        // Respaldo por distancia: si la pelota tapa el sensor de color, el
        // tile rojo no se ve y se cierra la seccion igual.
        if (ahora - tInicioAvance >= msPorCm(B_SALIDA_CM))
        {
            mov.detener();
            Serial.println(F("[PISTA B] Fin de seccion 1 por distancia (tile rojo no visto)."));
            cambiarEstado(EstadoB::B1_FIN);
        }
        break;
    }

    case EstadoB::B1_RECUPERANDO:
    {
        if (mov.isManeuverDone())
        {
            if (reintentos >= B_REINTENTOS_MAX)
            {
                mov.detener();
                Serial.println(F("[PISTA B] Atasco persistente. FIN."));
                cambiarEstado(EstadoB::B1_FIN);
                break;
            }

            reintentos++;
            mov.startForward();
            cambiarEstado(estadoRetorno);
        }
        break;
    }

    case EstadoB::B1_FIN:
        mov.detener();
        return true;
    }

    return false;
}

// ---------------------------------------------------------------- publico

void pistaB_init(Movimiento &mov, Garra &garra, uint8_t seccionInicial)
{
    mov.setVelocidad(VEL_B_PWM);
    mov.setVelocidadGiro(VEL_GIRO_PWM);

    // Arranca CERRADA: el ultrasonico frontal vive en la garra
    garra.cerrarSinEspera();

    seccion = (seccionInicial == 0) ? 1 : seccionInicial;

    if (seccion > 1)
    {
        // Secciones 2 y 3 todavia no estan implementadas.
        Serial.print(F("[PISTA B] Seccion "));
        Serial.print(seccion);
        Serial.println(F(" pendiente; arranco en la 1."));
        seccion = 1;
    }

    armado = false;
    pelotaEnGarra = false;
    reintentos = 0;
    pulsos = 0;
    estadoRetorno = EstadoB::B1_AVANZANDO;
    tArmado = 0;
    tCandidato = 0;
    tFrente = 0;
    tInicioSeccion = millis();
    tInicioAvance = millis();

    Serial.println(F("[PISTA B] Seccion 1: buscando la pelota (garra cerrada)."));

    cambiarEstado(EstadoB::B1_AVANZANDO);
    mov.startForward();
}

bool pistaB_ejecutar(Movimiento &mov, SensorColor &colorFrontal, Garra &garra)
{
    mov.update();

    if (seccion == 1)
    {
        return ejecutarSeccion1(mov, colorFrontal, garra);
    }

    return true;
}