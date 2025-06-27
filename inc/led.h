#ifndef LED_H
#define LED_H

#include <FreeRTOS.h>
#include <task.h>
#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "queue.h"

#define LED_0 25
#define LED_1 15
#define LED_2 16
#define LED_3 17    
#define LED_Sample_Rate 500 //ms
#define ON 1
#define OFF 0

//structs
typedef struct {
    uint led_pin;
    bool info;
    uint delay_ms;
    const char* task_name;
} BlinkParams_t;

void intro_task(void *pvParameters);

void led_task(void *pvParameters);

void Pulling_CPU_Usage_task(void *pvParameters);


#endif