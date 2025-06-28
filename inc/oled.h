#ifndef OLED_H
#define OLED_H

#include <FreeRTOS.h>
#include <task.h>
#include <stdio.h>
#include <string.h>

#include "pico/stdlib.h"
#include "i2c.h"
#include "hardware/i2c.h"

#include "fonts.h"

#define OLED_ADDR 0x3C

#define OLED_WIDTH 128
#define OLED_HEIGHT 64

void oled_send_command(uint8_t cmd);
void oled_init();
void oled_send_data(uint8_t *data, size_t len);
void oled_fill_screen();
void oled_write_string(uint8_t x, uint8_t page, const char *str);

#endif