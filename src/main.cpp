#include <Arduino.h>
#include <sensorUltrasonico.h>

SensorUltrasonico frontal(22, 23);

void setup() {
    Serial.begin(9600);

    frontal.begin();
}

void loop() {
    float distancia = frontal.getDistanceCm();

    Serial.println(distancia);

    delay(100);
}