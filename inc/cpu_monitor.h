#ifndef CPU_MONITOR_H
#define CPU_MONITOR_H

#include <FreeRTOS.h>
#include <task.h>
#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"


#define	MAX_FREERTOS_TASKS 10

char* get_state_task(TaskHandle_t handler_task);

void vApplicationIdleHook(void);

void cpu_measure_task(void *pvParameters);

TaskHandle_t *set_TaskHandler_Idex();

#endif