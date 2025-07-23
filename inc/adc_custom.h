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
#define ADC_RESOLUCAO 4095.0f
#define ADC_MAX_VALUE 4095 // Valor máximo do ADC (12 bits)
#define ADC_Queue_Size sizeof(int)

//umidade
#define ADC_Solo_Humidade 26 // GPIO26 (ADC0)

//exposição solar
#define ADC_Exposição_solar 27 // GPIO27 (ADC1)

//temperatura
#define ADC_Temperatura 28 // GPIO28 (ADC2) - Não utilizado neste projeto

void ADC_Read_Task(void *pvParameters);

#endif