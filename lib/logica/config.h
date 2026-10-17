#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>
#include "../SensorDeColor/sensorColor.h"

#define LED_1 51
#define LED_ON_MS 180
#define LED_OFF_MS 180

#define COLOR_A_1 ColorDetectado::AZUL
#define COLOR_A_2 ColorDetectado::AMARILLO
#define COLOR_A_3 ColorDetectado::NARANJA
#define COLOR_A_4 ColorDetectado::ROSA
#define COLOR_A_FINAL ColorDetectado::ROJO

#define DIST_LATERAL_IDEAL_CM 6.0f
#define DIST_PARED_LATERAL_CM 18.0f
#define DIST_FRENTE_PARADA_CM 8.0f

#define ULTRASONICO_MIN_CM 2.0f
#define ULTRASONICO_MAX_CM 200.0f
#define ULTRASONICO_TIMEOUT_US 25000UL
#define ULTRASONICO_INTERVALO_MS 40UL

#define VEL_AVANCE_PWM 75
#define VEL_GIRO_PWM 100
#define VEL_GIRO_MIN_PWM 80

#define TRIM_REAL_IZQ 1.00f
#define TRIM_REAL_DER 1.08f

#define SIGNO_GIRO (-1.0f)
#define GYRO_DEADBAND_DPS 0.28f
#define CALIB_MUESTRAS 300
#define CALIB_TIEMPO_MS 20

#define KP_RUMBO 1.5f
#define CORR_MAX 14
#define KP_PARED_GRADOS 4.0f
#define MAX_AJUSTE_RUMBO 12.0f

#define TOL_GIRO_GRADOS 0.5f
#define GIRO_ESTABLE_MS 80
#define GIRO_TIMEOUT_MS 3000
#define KP_GIRO 3.5f
#define PWM_GIRO_MAX_EXTRA 30

#define RETROCESO_PRE_GIRO_MS 150
#define RETROCESO_ATASCO_MS 300

#define COLOR_LECTURAS_CONSECUTIVAS 4
#define COLOR_INTERVALO_MS 25
#define COLOR_COOLDOWN_MS 1850

#define ATASCO_TIMEOUT_MS 3000
#define ATASCO_DELTA_MIN_CM 1.0f

#endif