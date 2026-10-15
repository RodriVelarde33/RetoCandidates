#include "sensorGiro.h"

// Constructor: inicializa las direcciones y las variables.
SensorGiro::SensorGiro(
    uint8_t mux, uint8_t canal, uint8_t mpu) {

    // Guarda la dirección del multiplexor recibida.
    direccionMux = mux;

    // Guarda el canal donde está conectado el sensor.
    canalMux = canal;

    // Guarda la dirección I2C del MPU6050.
    direccionMPU = mpu;

    // Inicializa las velocidades angulares en cero.
    giroX = 0;
    giroY = 0;
    giroZ = 0;

    // Inicializa las aceleraciones en cero.
    aceleracionX = 0;
    aceleracionY = 0;
    aceleracionZ = 0;
}

// Selecciona el canal del multiplexor donde está el MPU6050.
bool SensorGiro::seleccionarCanal() {

    // Inicia la comunicación con el multiplexor.
    Wire.beginTransmission(direccionMux);

    // Activa únicamente el canal indicado.
    // Por ejemplo, 1 << 2 produce 00000100,
    // lo que selecciona el canal 2.
    Wire.write(1 << canalMux);

    // Finaliza la comunicación y comprueba si hubo un error.
    // Devuelve true si la operación fue exitosa.
    return Wire.endTransmission() == 0;
}

// Escribe un valor en uno de los registros del MPU6050.
bool SensorGiro::escribirRegistro(
    uint8_t registro, uint8_t valor) {

    // Primero selecciona el canal del multiplexor.
    // Si falla, no intenta comunicarse con el sensor.
    if (!seleccionarCanal()) {
        return false;
    }

    // Inicia la comunicación con el MPU6050.
    Wire.beginTransmission(direccionMPU);

    // Indica qué registro queremos modificar.
    Wire.write(registro);

    // Envía el valor que se guardará en ese registro.
    Wire.write(valor);

    // Finaliza la comunicación.
    // Un resultado igual a cero significa que no hubo error.
    return Wire.endTransmission() == 0;
}

// Lee varios registros consecutivos del MPU6050.
bool SensorGiro::leerRegistros(
    uint8_t registro, uint8_t cantidad, uint8_t datos[]) {

    // Selecciona el canal donde se encuentra el sensor.
    if (!seleccionarCanal()) {
        return false;
    }

    // Inicia la comunicación con el MPU6050.
    Wire.beginTransmission(direccionMPU);

    // Indica la dirección del primer registro que se leerá.
    Wire.write(registro);

    // Finaliza la transmisión manteniendo el bus I2C activo.
    // Esto permite solicitar los datos a continuación.
    if (Wire.endTransmission(false) != 0) {
        return false;
    }

    // Solicita la cantidad de bytes indicada al sensor.
    // El último argumento indica que se finalice la transmisión.
    uint8_t recibidos = Wire.requestFrom(
        direccionMPU, cantidad, (uint8_t)true
    );

    // Comprueba que hayan llegado todos los bytes solicitados.
    if (recibidos != cantidad) {

        // Si faltan datos, descarta los bytes que queden pendientes.
        while (Wire.available()) {
            Wire.read();
        }

        // Indica que la lectura no se completó correctamente.
        return false;
    }

    // Copia los bytes recibidos al arreglo proporcionado.
    for (uint8_t i = 0; i < cantidad; i++) {
        datos[i] = Wire.read();
    }

    // Indica que la lectura se realizó correctamente.
    return true;
}

// Comprueba la identidad del sensor y configura su funcionamiento.
bool SensorGiro::begin() {

    // Arreglo para guardar el byte de identificación.
    uint8_t identidad[1];

    // Lee el registro WHO_AM_I, ubicado en la dirección 0x75.
    if (!leerRegistros(0x75, 1, identidad)) {
        return false;
    }

    // El MPU6050 normalmente devuelve 0x68 en este registro.
    // Si el valor es distinto, no continúa con la configuración.
    if (identidad[0] != 0x68) {
        return false;
    }

    // Registro PWR_MGMT_1 (0x6B).
    // Escribe cero para sacar al sensor del modo de suspensión.
    if (!escribirRegistro(0x6B, 0x00)) {
        return false;
    }

    // Registro GYRO_CONFIG (0x1B).
    // Configura el giroscopio para medir entre -250 y +250 grados/s.
    if (!escribirRegistro(0x1B, 0x00)) {
        return false;
    }

    // Registro ACCEL_CONFIG (0x1C).
    // Configura el acelerómetro para medir entre -2 g y +2 g.
    if (!escribirRegistro(0x1C, 0x00)) {
        return false;
    }

    // Si todas las operaciones fueron exitosas, termina la inicialización.
    return true;
}

// Obtiene nuevas mediciones del acelerómetro y el giroscopio.
bool SensorGiro::actualizar() {

    // El MPU6050 tiene 14 bytes consecutivos de datos:
    // 6 de aceleración, 2 de temperatura y 6 de velocidad angular.
    uint8_t datos[14];

    // Comienza a leer desde el registro 0x3B.
    // Si falla la lectura, no actualiza las mediciones.
    if (!leerRegistros(0x3B, 14, datos)) {
        return false;
    }

    // Une los dos bytes de la aceleración del eje X.
    // El primero contiene los bits más significativos
    // y el segundo contiene los menos significativos.
    int16_t ax = (int16_t)(
        ((uint16_t)datos[0] << 8) | datos[1]
    );

    // Une los dos bytes de la aceleración del eje Y.
    int16_t ay = (int16_t)(
        ((uint16_t)datos[2] << 8) | datos[3]
    );

    // Une los dos bytes de la aceleración del eje Z.
    int16_t az = (int16_t)(
        ((uint16_t)datos[4] << 8) | datos[5]
    );

    // Los bytes 6 y 7 corresponden a la temperatura.
    // No los utilizamos en esta versión del programa.

    // Une los dos bytes de la velocidad angular del eje X.
    int16_t gx = (int16_t)(
        ((uint16_t)datos[8] << 8) | datos[9]
    );

    // Une los dos bytes de la velocidad angular del eje Y.
    int16_t gy = (int16_t)(
        ((uint16_t)datos[10] << 8) | datos[11]
    );

    // Une los dos bytes de la velocidad angular del eje Z.
    int16_t gz = (int16_t)(
        ((uint16_t)datos[12] << 8) | datos[13]
    );

    // Convierte la aceleración original a unidades de gravedad.
    // Con el rango de +/- 2 g, cada unidad equivale a 16384 cuentas.
    aceleracionX = ax / 16384.0;
    aceleracionY = ay / 16384.0;
    aceleracionZ = az / 16384.0;

    // Convierte las lecturas del giroscopio a grados por segundo.
    // Con el rango de +/- 250 grados/s, se divide entre 131.
    giroX = gx / 131.0;
    giroY = gy / 131.0;
    giroZ = gz / 131.0;

    // Indica que las mediciones se actualizaron correctamente.
    return true;
}

// Devuelve la última velocidad angular calculada del eje X.
float SensorGiro::getGiroX() {
    return giroX;
}

// Devuelve la última velocidad angular calculada del eje Y.
float SensorGiro::getGiroY() {
    return giroY;
}

// Devuelve la última velocidad angular calculada del eje Z.
float SensorGiro::getGiroZ() {
    return giroZ;
}

// Devuelve la última aceleración calculada del eje X.
float SensorGiro::getAceleracionX() {
    return aceleracionX;
}

// Devuelve la última aceleración calculada del eje Y.
float SensorGiro::getAceleracionY() {
    return aceleracionY;
}

// Devuelve la última aceleración calculada del eje Z.
float SensorGiro::getAceleracionZ() {
    return aceleracionZ;
}