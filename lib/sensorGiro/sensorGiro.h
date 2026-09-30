#ifndef SENSOR_GIRO_H
#define SENSOR_GIRO_H

#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>

class Giroscopio
{
public:
    Giroscopio();

private:
    Adafruit_MPU6050 mpu;
};

#endif