#include <Arduino.h>
#include <sensorColor.h>

SensorColor sensorColor;

void setup()
{
    Serial.begin(9600);

    if (sensorColor.begin())
    {
        Serial.println("Sensor de color inicializado");
    }
    else
    {
        Serial.println("Error al inicializar el sensor de color");
    }
}

void loop()
{
    ColorDetectado color = sensorColor.detectarColorNuevo();

    if (color == CIAN)
    {
        Serial.println("CIAN");
    }
    else if (color == AMARILLO)
    {
        Serial.println("AMARILLO");
    }
    else if (color == NARANJA)
    {
        Serial.println("NARANJA");
    }
    else if (color == MAGENTA)
    {
        Serial.println("MAGENTA");
    }

    delay(200);
}

