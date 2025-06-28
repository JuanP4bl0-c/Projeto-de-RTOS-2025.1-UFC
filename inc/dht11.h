#ifndef DHT11_H
#define DHT11_H

#include <stdint.h>
#include <stdbool.h>
#include "oled.h"
#include <FreeRTOS.h>
#include "pico/stdlib.h"

typedef struct {
    uint led_pin;
    uint temperatura;
    uint humidade;
} dht11_params_t;


bool dht11_read(uint gpio, uint8_t *temperature, uint8_t *humidity);
void dht11_task(void *params);

#endif