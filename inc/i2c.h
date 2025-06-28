#ifndef I2C_H
#define I2C_H

#include <stdint.h>
#include <stddef.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"

#define I2C_PORT i2c0
#define I2C_SDA_PIN 4
#define I2C_SCL_PIN 5

void i2c_init_custom(void);
int i2c_write(uint8_t addr, uint8_t *data, size_t len);

#endif