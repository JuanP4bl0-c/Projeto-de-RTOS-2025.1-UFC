#ifndef WIFI_H
#define WIFI_H

#include <FreeRTOS.h>
#include <task.h>
#include <stdio.h>
#include <string.h>

#include "pico/stdlib.h"
#include "hardware/uart.h"

#define ESP8266_UART_ID uart1
#define ESP8266_BAUDRATE 115200
#define ESP8266_TX_PIN 4
#define ESP8266_RX_PIN 5

void esp8266_uart_init(void);
void esp8266_send_cmd(const char *cmd);
void esp8266_read_response(char *buffer, size_t maxlen);
void esp8266_connect_wifi(const char *ssid, const char *password);