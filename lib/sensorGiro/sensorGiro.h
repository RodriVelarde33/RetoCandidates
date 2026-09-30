#ifndef SENSOR_GIRO_H
#define SENSOR_GIRO_H

#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>

class Giroscopio
{
public:
    Giroscopio();
    bool inicializar();
    // esta funcion se llama cuando el robot esta completamente quieto, para que el sensor pueda calibrarse y obtener un valor de referencia
    void calibrar();
    // esta funcion se llama en cada iteracion del loop principal, para actualizar el valor del angulo actual
    void actualizar();
    // esta funcion devuelve el angulo actual del robot, en grados, con respecto a la posicion inicial
    float anguloActual();
    // este es para ajustar el angulo a 0, es de mucha ayuda cuando queremos hacer un giro
    void resetAngulo();

private:
    Adafruit_MPU6050 mpu;
};

#endif