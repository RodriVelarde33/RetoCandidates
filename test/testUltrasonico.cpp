#include <Arduino.h>
#include <sensorUltrasonico.h>

//trig, echo
SensorUltrasonico frontal(30, 31);

void setup() {
    Serial.begin(9600);

    frontal.begin();
}

void loop() {
    float distancia = frontal.getDistanceCm();

    
    Serial.print("Distancia: ");
    Serial.print(distancia);
    Serial.println(" cm");
    
    

    delay(100);
}