#ifndef ULTRASONIC_H
#define ULTRASONIC_H

#include <Arduino.h>

class SensorUltrasonico {
private:
    uint8_t trigPin; //uint8_t porque son enteros pequeños, 0-255 bits
    uint8_t echoPin;

public:
    SensorUltrasonico(uint8_t trig, uint8_t echo);

    void begin();
    float getDistanceCm();

};

#endif


