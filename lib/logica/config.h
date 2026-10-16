#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>
#include "../SensorDeColor/sensorColor.h"

// ---------------- LED ----------------
#define LED_1 51
#define LED_ON_MS 180
#define LED_OFF_MS 180

// ---------------- Colores Pista A ----------------
#define COLOR_A_1 ColorDetectado::AZUL
#define COLOR_A_2 ColorDetectado::AMARILLO
#define COLOR_A_3 ColorDetectado::NARANJA
#define COLOR_A_4 ColorDetectado::ROSA
#define COLOR_A_FINAL ColorDetectado::ROJO

// ---------------- Distancias ----------------
#define DIST_LATERAL_IDEAL_CM 6.0f
#define DIST_PARED_LATERAL_CM 15.0f
#define DIST_FRENTE_PARADA_CM 14.0f
#define DIST_FRENTE_CONFIRMADA_CM 12.0f

#define ULTRASONICO_MIN_CM 2.0f
#define ULTRASONICO_MAX_CM 200.0f

// ---------------- Velocidades ----------------
#define VEL_AVANCE_PWM 65
#define VEL_GIRO_PWM 115
#define VEL_GIRO_MIN_PWM 95
#define VEL_CENTRADO_PWM 70
#define VEL_RECUPERACION_PWM 75

// ---------------- Trim motores ----------------
#define TRIM_REAL_IZQ 1.00f
#define TRIM_REAL_DER 1.05f

// ---------------- Giroscopio ----------------
#define SIGNO_GIRO (-1.0f)
#define GYRO_DEADBAND_DPS 0.28f // promedio (IA 0.20, mio 0.35)
#define CALIB_MUESTRAS 300
#define CALIB_TIEMPO_MS 20

// ---------------- Control de rumbo ----------------
#define KP_RUMBO 1.85f // promedio (IA 2.2, mio 1.5)
#define CORR_MAX 14
#define KP_PARED_GRADOS 1.65f // promedio (IA 1.8, mio 1.5)
#define MAX_AJUSTE_RUMBO 6.5f // promedio (IA 7.0, mio 6.0)

// ---------------- Giros ----------------
#define TOL_GIRO_GRADOS 0.65f // promedio (IA 0.8, mio 0.5)
#define GIRO_ESTABLE_MS 60
#define GIRO_TIMEOUT_MS 3000  // promedio (IA 2500, mio 3500)
#define KP_GIRO 5.0f          // promedio (IA 7.0, mio 3.0)
#define PWM_GIRO_MAX_EXTRA 30 // promedio (IA 35, mio 25)

// ---------------- Centrado ----------------
#define TOL_CENTRADO_CM 1.1f     // promedio (IA 0.7, mio 1.5)
#define CENTRADO_TIMEOUT_MS 1400 // promedio (IA 1200, mio 1600)
#define KP_CENTRADO 16.0f        // promedio (IA 22.0, mio 10.0)

// ---------------- Retroceso pre-giro ----------------
#define RETROCESO_PRE_GIRO_MS 220
#define RETROCESO_ATASCO_MS 350

// ---------------- Color ----------------
#define COLOR_LECTURAS_CONSECUTIVAS 4
#define COLOR_INTERVALO_MS 25
#define COLOR_COOLDOWN_MS 1850 // promedio (IA 1200, mio 2500)

// ---------------- Atasco ----------------
#define ATASCO_TIMEOUT_MS 1800
#define ATASCO_DELTA_MIN_CM 1.0f
#define ATASCO_MAX_REINTENTOS 3

#endif