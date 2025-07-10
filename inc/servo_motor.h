#ifndef SERVO_MOTOR_H
#define SERVO_MOTOR_H

#include <FreeRTOS.h>
#include <task.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include <stdio.h>


#define SERVO_PIN 15 // GPIO pin for the servo motor
#define SERVO_PWM_FREQ 50 // Frequency for PWM (50Hz for servos)

void servo_init(uint gpio);
void servo_set_angle(uint gpio, uint angle);
void servo_task_test(void *pvParameters);

#endif