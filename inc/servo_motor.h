#ifndef SERVO_MOTOR_H
#define SERVO_MOTOR_H

#include <FreeRTOS.h>
#include <task.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include <stdio.h>


#define SERVO_PIN 18 // GPIO pin for the servo motor
#define SERVO_PWM_FREQ 50 // Frequency for PWM (50Hz for servos)

extern uint16_t angle_atual; // Variável global para armazenar o ângulo atual do servo motor

void servo_init(uint gpio);
void servo_set_angle(uint gpio, uint angle);
void servo_move_to_angle(uint gpio, uint y, uint delay_ms);
void servo_task_test(void *pvParameters);
void reset_servo(uint gpio);
void close_servo(uint gpio);
void open_servo(uint gpio);
#endif