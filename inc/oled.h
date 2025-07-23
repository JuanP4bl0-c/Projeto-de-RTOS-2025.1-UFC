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
#define OLED_CLS 0x00 // Clear screen command

void oled_send_command(uint8_t cmd);
void oled_init();
void oled_send_data(uint8_t *data, size_t len);
void oled_fill_screen();
void oled_write_string(uint8_t x, uint8_t page, const char *str);
void print_oled_stats(uint8_t humidade_percent, uint8_t exposicao_percent, uint16_t exposicao_solar_ideal,uint8_t temperatura, const char* nome);
void oled_power_off(void);
void oled_power_on(void);
void uint8_to_string(uint8_t value, char *str);

#endif