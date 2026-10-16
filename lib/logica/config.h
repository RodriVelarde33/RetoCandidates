#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>
#include "../SensorDeColor/sensorColor.h"

// LED
#define LED_1 51
#define LED_ON_MS 200
#define LED_OFF_MS 200

// Colores de la pista
#define COLOR_A_1 ColorDetectado::AZUL
#define COLOR_A_2 ColorDetectado::AMARILLO
#define COLOR_A_3 ColorDetectado::NARANJA
#define COLOR_A_4 ColorDetectado::ROSA
#define COLOR_A_FINAL ColorDetectado::ROJO

// Distancias (cm)
#define DIST_LATERAL_IDEAL_CM 6.0f
#define DIST_PARED_LATERAL_CM 15.0f
#define DIST_FRENTE_ALTO_CM 15.0f
#define DIST_CENTRAR_CM 21.0f

// Velocidades
#define VEL_AVANCE_PWM 120
#define VEL_GIRO_PWM 120
#define VEL_GIRO_MIN_PWM 100

#define VEL_AVANCE_CM_S 20.0f
#define TIEMPO_CENTRAR_MS ((unsigned long)(1000.0f * DIST_CENTRAR_CM / VEL_AVANCE_CM_S))

// Control
#define TRIM_REAL_IZQ 1.0f
#define TRIM_REAL_DER 1.05f

#define SIGNO_GIRO (-1.0f)

#define KP_RUMBO 1.5f
#define CORR_MAX 12
#define KP_PARED_GRADOS 1.5f
#define MAX_AJUSTE_RUMBO 6.0f

#define KP_GIRO 1.0f
#define TOL_GIRO_GRADOS 0.3f

#define LECTURAS_SIN_COLOR 3

#endif