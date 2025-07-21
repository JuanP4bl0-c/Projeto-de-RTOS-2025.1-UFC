#include "i2c.h"
#include "pico/stdlib.h"
#include "hardware/i2c.h"

void i2c_init_custom(void) {
    i2c_init(I2C_PORT, 400 * 1000); // 400kHz
    gpio_set_function(I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA_PIN);
    gpio_pull_up(I2C_SCL_PIN);
}

int i2c_write(uint8_t addr, uint8_t *data, size_t len) {
    int ret = i2c_write_blocking(I2C_PORT, addr, data, len, false);
    return ret;
}