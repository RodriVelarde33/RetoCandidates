/*Aqui ira la logica del movimiento del robot, basado en las lecturas que nos den los sensores llamaremos a este archivo movimiento.cpp,y en este archivo
podremos usar las funciones de control  como girar ala izquierda o derecha
y tambien podremos usar funciones de control de velocidad, para poder hacer que el robot se mueva mas rapido o mas lento
dependiendo de la situacion.
*/
#include "movimiento.h"

// Constructor
Movimiento::Movimiento(PuenteH &puente, SensorUltrasonico &frontal, SensorColor &colorFrontal, Garra &garra)
    : puente(puente),
      frontal(frontal),
      colorFrontal(colorFrontal),
      garra(garra)

{
    velocidad = 100;
}

// Inicializar componentes
void Movimiento::begin()
{
    puente.begin();
    frontal.begin();
    colorFrontal.begin();

    detener();
}

// Avanzar
void Movimiento::avanzar()
{
    puente.izquierdaAdelante(velocidad);
    puente.derechaAdelante(velocidad);
}

// Retroceder
void Movimiento::retroceder()
{
    puente.izquierdaAtras(velocidad);
    puente.derechaAtras(velocidad);
}

// Detener
void Movimiento::detener()
{
    puente.detener();
}

// Leer sensores y decidir movimiento
void Movimiento::actualizar()
{
    float distanciaFrontal = frontal.getDistanceCm();

    // Si no hay lectura válida, detener por seguridad
    if (distanciaFrontal == -1)
    {
        detener();
        return;
    }

    // Si hay un obstáculo cerca, detener y dar una vuelta a la derecha
    if (distanciaFrontal < DISTANCIA_MINIMA)
    {
        detener();
        puente.derechaAtras(100);
        puente.izquierdaAdelante(100);
        delay(900); // Girar durante 0.9 segundos
        detener();
    }
    else
    {
        avanzar();
    }

    if (colorFrontal.detectarColorNuevo() == ColorDetectado::NARANJA)
    {
        detener();
        delay(1000); // Esperar 1 segundo
        retroceder();
        delay(1000); // Retroceder durante 1 segundo
        detener();
        delay(500);
        garra.abrir();
        delay(1000); // Esperar 1 segundo
        avanzar();
        delay(1000); // Avanzar durante 1 segundo
        detener();
        garra.cerrar();
    }
}