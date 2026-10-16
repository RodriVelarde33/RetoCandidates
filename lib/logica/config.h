#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>
#include "../SensorDeColor/sensorColor.h"

// LED
#define LED_1 51
#define LED_ON_MS 200
#define LED_OFF_MS 200

// Colores pista A
#define COLOR_A_1 ColorDetectado::AZUL
#define COLOR_A_2 ColorDetectado::AMARILLO
#define COLOR_A_3 ColorDetectado::NARANJA
#define COLOR_A_4 ColorDetectado::ROSA
#define COLOR_A_FINAL ColorDetectado::ROJO

// Distancias (cm)
#define DIST_LATERAL_IDEAL_CM 6.0f
#define DIST_PARED_LATERAL_CM 15.0f
#define DIST_FRENTE_PARADA_CM 15.0f

// Velocidades
#define VEL_AVANCE_PWM 60
#define VEL_GIRO_PWM 120
#define VEL_GIRO_MIN_PWM 100

// Compensacion de motores
#define TRIM_REAL_IZQ 1.0f
#define TRIM_REAL_DER 1.05f

// Giroscopio
#define SIGNO_GIRO (-1.0f)

// Control
#define KP_RUMBO 1.5f
#define CORR_MAX 12
#define KP_PARED_GRADOS 1.5f
#define MAX_AJUSTE_RUMBO 6.0f
#define TOL_GIRO_GRADOS 0.3f

#endif