#include <Arduino.h>
#include <Wire.h>
#include "sensorColor.h"

// Definición de los sensores según tu diseño:
// Canal 3: Sensor de Piso
// Canal 4: Sensor de Frente
SensorColor sensorPiso(3);
SensorColor sensorFrente(5);

// Función auxiliar para imprimir el color detectado en texto legible
const char *colorToString(ColorDetectado color)
{
    switch (color)
    {
    case CIAN:
        return "CIAN";
    case AMARILLO:
        return "AMARILLO";
    case NARANJA:
        return "NARANJA";
    case MAGENTA:
        return "MAGENTA";
    default:
        return "NINGUNO";
    }
}

void setup()
{
    Serial.begin(115200);
    delay(1000);

    Wire.begin(); // Inicializa I2C

    Serial.println("\n--- INICIANDO SISTEMA DE COLOR (REGISTROS DIRECTOS) ---");

    // Inicializar Sensor de Piso (Canal 3)
    if (sensorPiso.begin())
    {
        Serial.println("¡Sensor de Piso (Canal 3) conectado!");
    }
    else
    {
        Serial.println("Error: No se detecta el Sensor de Piso en el Canal 3.");
    }

    // Inicializar Sensor de Frente (Canal 4)
    if (sensorFrente.begin())
    {
        Serial.println("¡Sensor de Frente (Canal 4) conectado!");
    }
    else
    {
        Serial.println("Error: No se detecta el Sensor de Frente en el Canal 4.");
    }

    Serial.println("-----------------------------------------------------");
}

void loop()
{
    float rPiso, gPiso, bPiso;
    float rFrente, gFrente, bFrente;

    // Obtener valores normalizados y colores de ambos sensores
    sensorPiso.leerRGBNormalizado(rPiso, gPiso, bPiso);
    ColorDetectado colorPiso = sensorPiso.leerColorActual();

    sensorFrente.leerRGBNormalizado(rFrente, gFrente, bFrente);
    ColorDetectado colorFrente = sensorFrente.leerColorActual();

    // Mostrar en Monitor Serie
    Serial.print("Piso  -> R:");
    Serial.print(rPiso, 2);
    Serial.print(" G:");
    Serial.print(gPiso, 2);
    Serial.print(" B:");
    Serial.print(bPiso, 2);
    Serial.print(" [");
    Serial.print(colorToString(colorPiso));
    Serial.print("]   ||   ");

    Serial.print("Frente -> R:");
    Serial.print(rFrente, 2);
    Serial.print(" G:");
    Serial.print(gFrente, 2);
    Serial.print(" B:");
    Serial.print(bFrente, 2);
    Serial.print(" [");
    Serial.print(colorToString(colorFrente));
    Serial.println("]");

    delay(250);
}