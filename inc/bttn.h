#ifndef BTTN_H
#define BTTN_H

#include <FreeRTOS.h>
#include <task.h>
#include <stdio.h>
#include <string.h>
#include "hardware/adc.h"
#include "pico/stdlib.h"
#include "led.h"
#include "queue.h"
#include "semphr.h"

#define Queue_Lenght 10

//pino bttn
extern QueueHandle_t BTTN_Queue;
extern SemaphoreHandle_t xButtonSemaphore;

#define BTTN_PIN_0 16
#define BTTN_PIN_1 17
#define BTTN_DELAY 100
#define BTTN_Queue_Size sizeof(uint32_t)

typedef struct{
    uint pin;
    uint debouce_delay;
    uint command;

}Bttn_Params_t;

void gpio_callback(uint gpio, uint32_t events);

void Button_task(void *pvParameters);

void process_button_task(void *pvParameters);

void semaforo_check(void* pvParameters);

#endif