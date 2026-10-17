#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>
#include "../SensorDeColor/sensorColor.h"

// LED
#define LED_1 51
#define LED_ON_MS 180
#define LED_OFF_MS 180

// Colores Pista A
#define COLOR_A_1 ColorDetectado::AZUL
#define COLOR_A_2 ColorDetectado::AMARILLO
#define COLOR_A_3 ColorDetectado::NARANJA
#define COLOR_A_4 ColorDetectado::ROSA
#define COLOR_A_FINAL ColorDetectado::ROJO

// Distancias (cm)
#define DIST_LATERAL_IDEAL_CM 6.0f
#define DIST_PARED_LATERAL_CM 15.0f
#define DIST_FRENTE_PARADA_CM 14.0f
#define DIST_FRENTE_CONFIRMADA_CM 12.0f

// Velocidades
#define VEL_AVANCE_PWM 65
#define VEL_GIRO_PWM 115
#define VEL_GIRO_MIN_PWM 95
#define VEL_CENTRADO_PWM 70

// Compensacion de motores
#define TRIM_REAL_IZQ 1.00f
#define TRIM_REAL_DER 1.05f

// Giroscopio
#define SIGNO_GIRO (-1.0f)
#define GYRO_DEADBAND_DPS 0.28f
#define CALIB_MUESTRAS 300
#define CALIB_TIEMPO_MS 20

// Control de rumbo
#define KP_RUMBO 1.85f
#define CORR_MAX 14
#define KP_PARED_GRADOS 1.65f
#define MAX_AJUSTE_RUMBO 6.5f

// Giros
#define TOL_GIRO_GRADOS 0.65f
#define GIRO_ESTABLE_MS 60
#define GIRO_TIMEOUT_MS 3000
#define KP_GIRO 3.5f
#define PWM_GIRO_MAX_EXTRA 30

// Centrado
#define TOL_CENTRADO_CM 1.1f
#define CENTRADO_TIMEOUT_MS 1400
#define KP_CENTRADO 12.0f

// Retroceso
#define RETROCESO_PRE_GIRO_MS 220
#define RETROCESO_ATASCO_MS 350

// Color
#define COLOR_LECTURAS_CONSECUTIVAS 4
#define COLOR_INTERVALO_MS 25
#define COLOR_COOLDOWN_MS 1850

// Atasco
#define ATASCO_TIMEOUT_MS 3000
#define ATASCO_DELTA_MIN_CM 1.0f

// ===== PISTA B =====

// Seleccion de pista al encender: jumper de este pin a GND = Pista B
#define PIN_SELECTOR_PISTA    4

// Garra
#define GARRA_MOVER_MS        900   // tiempo que tarda el servo en moverse

// Velocidades
#define VEL_B_PWM             50    // avance en Pista B
#define VEL_B_AGARRE_PWM      40    // acercamiento lento a la pelota

// Calibracion de distancia: ms que tarda en recorrer 1 cm a VEL_B_PWM.
// Medir en pista: avanzar 30 cm, cronometrar y dividir entre 30.
#define B_MS_POR_CM           50
// Lo mismo pero a VEL_B_AGARRE_PWM (el avance lento tarda mas por cm).
// Medir aparte: avanzar 30 cm a velocidad de agarre y dividir entre 30.
#define B_MS_POR_CM_LENTO     63

// Seccion 1: anillo 3x3 alrededor de la celda central con la pelota
#define B_LADO_PELOTA         (+1)  // +1 = celda al lado derecho, -1 = izquierdo
#define B_PARED_LATERAL_CM    12.0f // hay pared lateral si la lectura es menor
#define B_APERTURA_CM         20.0f // hay hueco si la lectura es mayor (o -1)
#define B_APERTURA_MS         180   // tiempo que debe sostenerse el hueco
#define B_FRENTE_CONFIRMA_MS  120   // tiempo que debe sostenerse la pared frontal
#define B_REINTENTOS_MAX      3     // reintentos tras atasco antes de rendirse
#define B_PARED_FRENTE_CM     13.0f // pared al frente: esquina del anillo
#define B_CENTRAR_CM          15.0f // avance para quedar al centro del hueco
#define B_ENTRADA_CM          12.0f // avance hasta la pelota dentro de la celda
#define B_DIST_PELOTA_CM      6.0f  // el ultrasonico la ve: frenar antes

// Confirmacion de la pelota con el sensor de color (frente de la garra).
// La garra SOLO se abre cuando ve el naranja de la pelota.
#define COLOR_B_PELOTA        ColorDetectado::NARANJA
#define B_ESPERA_COLOR_MS     180   // asentado del robot antes de leer color
#define B_PULSOS_ALINEAR      4     // pulsos cortos buscando el naranja
#define B_PULSO_ALINEAR_CM    2.0f  // largo de cada pulso
#define B_AVANCE_AGARRE_CM    5.0f  // avance final a ciegas con garra abierta
#define B_RETROCESO_CM        25.0f // retroceso para salir de la celda

#define B_SALIDA_CM           60.0f // avance buscando el checkpoint 1
#define B_TIMEOUT_S1_MS       90000UL

#define COLOR_B_CHECKPOINT    ColorDetectado::ROJO

#endif