#ifndef BLUETHOOTH_H
#define BLUETHOOTH_H

#include <FreeRTOS.h>
#include <task.h>
#include "pico/stdlib.h"
#include "hardware/uart.h"
#include <stdio.h>

#define BLUETHOOTH_TX_PIN 0
#define BLUETHOOTH_RX_PIN 1
#define BAUD_RATE 9600
#define UART_ID uart0

void Bluethooth_Setup();

void bluetooth_send(const char *msg);

void Task_Bluetooth(void *pvParameters);

void Task_Bluetooth_Receive(void *pvParameters);

#endif