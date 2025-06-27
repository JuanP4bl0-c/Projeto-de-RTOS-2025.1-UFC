#ifndef ADC_CUSTOM_H
#define ADC_CUSTOM_H

#include <FreeRTOS.h>
#include <task.h>
#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "queue.h"
#include "led.h"
#include "hardware/adc.h"

#include "semphr.h"

//pino adc
#define Queue_Lenght 10
#define ADC_PIN 26
#define ADC_INPUT 0
#define ADC_VOLT_FER 3.3
#define ADC_DELAY 300
#define ADC_RESOLUCAO 4095.0f
#define ADC_Queue_Size sizeof(int)

//RGB LED
#define RBG_PIN_R 13
#define RBG_PIN_G 14
#define RBG_PIN_B 15

//acelerometro
#define task_read_rate 100 // ms
#define task_deadline_rate 500 // ms
#define task_deadline_rate_max 1000 // ms
#define Voltage_ref 3.3
#define Voltage_mant 100

typedef struct {
    uint gpio;
    uint delay_ms;
    uint deadline_ms;
    uint16_t valor;
} Acelerometro_Params_t;

extern SemaphoreHandle_t Printf_mutex;

void ADC_Read_Task(void *pvParameters);

void RGB_Led_task(void *pvParameters);

void acelerometro_read_task(void *pvParameters);

void acelerometro_print_task(void *pvParameters);

void acelerometro_volt_print_convert_task(void *pvParameters);

#endif