#include <Arduino.h>
#include <Wire.h>

#define TCA_ADDR 0x70 // Dirección I2C estándar del multiplexor TCA9548A

// Función para cambiar de canal en el multiplexor
void tcaSelect(uint8_t canal)
{
  if (canal > 7)
    return;
  Wire.beginTransmission(TCA_ADDR);
  Wire.write(1 << canal);
  Wire.endTransmission();
}

void setup()
{
  Serial.begin(115200);
  while (!Serial)
    ;

  Wire.begin(); // Inicializa I2C (Pines 20 y 21 en tu Arduino)
  Serial.println("\n--- INICIANDO TEST DE MULTIPLEXOR ---");
}

void loop()
{
  // 1. Probar Canal 2 (Giroscopio)
  tcaSelect(2);
  Wire.beginTransmission(0x68); // Cambia 0x68 por la dirección I2C de tu giroscopio (ej. MPU6050)
  byte errorGiro = Wire.endTransmission();

  Serial.print("Canal 2 (Giroscopio): ");
  if (errorGiro == 0)
  {
    Serial.println("¡Conectado y respondiendo!");
  }
  else
  {
    Serial.println("No se detecta dispositivo (revisa conexiones).");
  }

  // 2. Probar Canal 3 (Sensor de color abajo)
  tcaSelect(3);
  Wire.beginTransmission(0x29); // 0x29 suele ser la dirección I2C del TCS34725
  byte errorAbajo = Wire.endTransmission();

  Serial.print("Canal 3 (Color Abajo): ");
  if (errorAbajo == 0)
  {
    Serial.println("¡Conectado y respondiendo!");
  }
  else
  {
    Serial.println("No se detecta dispositivo.");
  }

  // 3. Probar Canal 4 (Sensor de color enfrente)
  tcaSelect(4);
  Wire.beginTransmission(0x29); // Misma dirección, pero el multiplexor aísla el canal
  byte errorFrente = Wire.endTransmission();

  Serial.print("Canal 4 (Color Frente): ");
  if (errorFrente == 0)
  {
    Serial.println("¡Conectado y respondiendo!");
  }
  else
  {
    Serial.println("No se detecta dispositivo.");
  }

  Serial.println("----------------------------------------");
  delay(2000); // Espera 2 segundos entre cada prueba
}