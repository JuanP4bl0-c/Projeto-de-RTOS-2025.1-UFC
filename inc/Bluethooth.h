#ifndef BLUETHOOTH_H
#define BLUETHOOTH_H

#include <FreeRTOS.h>
#include <task.h>
#include "pico/stdlib.h"
#include "hardware/uart.h"
#include <stdio.h>

#define BLUETHOOTH_TX_PIN 4
#define BLUETHOOTH_RX_PIN 5
#define BAUD_RATE 9600
#define UART_ID uart1

void Bluethooth_Setup();

void bluetooth_send(const char *msg);

void Task_Bluetooth(void *pvParameters);

void Task_Bluetooth_Receive(void *pvParameters);

#endif