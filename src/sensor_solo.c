#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/i2c.h"
#include "hardware/gpio.h"
#include "pico/time.h"
#include <stdio.h>
#include <math.h>
#include "ssd1306.h"


#define PIN_SOIL_CAP_ADC   27   // GP27 ADC1
#define PIN_SOIL_RES_ADC   26   // GP26 ADC0
#define PIN_SOIL_RES_PWR   16   // GP16

#define PIN_BUTTON_LEFT    20
#define PIN_BUTTON_MIDDLE  21
#define PIN_BUTTON_RIGHT   22

#define I2C_PORT           i2c0
#define I2C_SDA_PIN        0    // GP0
#define I2C_SCL_PIN        1    // GP1

#define LOOPS_PER_RES_MEAS 20
#define LOOP_INTERVAL_MS   500
#define RES_SETTLE_MS      50
#define NUM_SAMPLES        500

const uint16_t CAP_RAW_DRY = 38000;
const uint16_t CAP_RAW_WET = 22000;
const uint16_t RES_RAW_DRY = 65535;
const uint16_t RES_RAW_WET = 10000;

typedef enum { MODEL_LINEAR, MODEL_DATAFIT1 } model_t;
typedef enum { SENSOR_RES, SENSOR_CAP } type_t;

static uint32_t adc_read_avg(int channel, int samples) {
    adc_select_input(channel);
    uint32_t sum = 0;
    for(int i = 0; i < samples; i++) {
        sum += adc_read();
    }
    return sum / samples;
}

double linear_conv(uint32_t raw, uint16_t raw_dry, uint16_t raw_wet) {
    double lower = fmin(raw_dry, raw_wet);
    double range = fabs((double)raw_wet - raw_dry);
    double fract = (raw - lower) / range;
    if(raw_dry > raw_wet) fract = 1.0 - fract;
    if(fract < 0) fract = 0;
    if(fract > 1) fract = 1;
    return fract * 100.0;
}

double datafit1_conv(type_t t, uint32_t raw) {
    if(t == SENSOR_CAP)
        return linear_conv(raw, CAP_RAW_DRY, CAP_RAW_WET);
    else
        return linear_conv(raw, RES_RAW_DRY, RES_RAW_WET);
}

void hardware_init() {
    stdio_init_all();
    adc_init();

    gpio_init(PIN_SOIL_RES_PWR);
    gpio_set_dir(PIN_SOIL_RES_PWR, GPIO_OUT);

    gpio_init(PIN_BUTTON_LEFT);
    gpio_set_dir(PIN_BUTTON_LEFT, GPIO_IN);
    gpio_pull_up(PIN_BUTTON_LEFT);

    gpio_init(PIN_BUTTON_MIDDLE);
    gpio_set_dir(PIN_BUTTON_MIDDLE, GPIO_IN);
    gpio_pull_up(PIN_BUTTON_MIDDLE);

    gpio_init(PIN_BUTTON_RIGHT);
    gpio_set_dir(PIN_BUTTON_RIGHT, GPIO_IN);
    gpio_pull_up(PIN_BUTTON_RIGHT);

    i2c_init(I2C_PORT, 400000);
    gpio_set_function(I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA_PIN);
    gpio_pull_up(I2C_SCL_PIN);

    ssd1306_init();
}

int main() {
    hardware_init();

    model_t model_idx = MODEL_LINEAR;
    bool raw_mode = false;
    bool show_output = true;
    int loop_count = 0;

    while(true) {
        loop_count++;

        uint32_t cap_raw = adc_read_avg(1, NUM_SAMPLES);

        uint32_t res_raw = 0;
        if(loop_count % LOOPS_PER_RES_MEAS == 1) {
            gpio_put(PIN_SOIL_RES_PWR, 1);
            sleep_ms(RES_SETTLE_MS);
            res_raw = adc_read_avg(0, NUM_SAMPLES);
            gpio_put(PIN_SOIL_RES_PWR, 0);
        }

        double res_pct = (model_idx == MODEL_LINEAR)
                         ? linear_conv(res_raw, RES_RAW_DRY, RES_RAW_WET)
                         : datafit1_conv(SENSOR_RES, res_raw);
        double cap_pct = (model_idx == MODEL_LINEAR)
                         ? linear_conv(cap_raw, CAP_RAW_DRY, CAP_RAW_WET)
                         : datafit1_conv(SENSOR_CAP, cap_raw);

        if(show_output) {
            if(raw_mode)
                printf("Raw R:%5u C:%5u\n", res_raw, cap_raw);
            else
                printf("Pct R:%3.0f%% C:%3.0f%%\n", res_pct, cap_pct);
        }

        ssd1306_clear();
        if(raw_mode) {
            ssd1306_printf(0, 0, "R raw:%5u", res_raw);
            ssd1306_printf(0,10, "C raw:%5u", cap_raw);
        } else {
            ssd1306_printf(0, 0, "R:%3.0f%%", res_pct);
            ssd1306_printf(64, 0, "C:%3.0f%%", cap_pct);
        }
        ssd1306_show();

        if(!gpio_get(PIN_BUTTON_LEFT)) {
            model_idx = (model_idx == MODEL_LINEAR ? MODEL_DATAFIT1 : MODEL_LINEAR);
            sleep_ms(200);
        }
        if(!gpio_get(PIN_BUTTON_MIDDLE)) {
            show_output = !show_output;
            sleep_ms(200);
        }
        if(!gpio_get(PIN_BUTTON_RIGHT)) {
            raw_mode = !raw_mode;
            sleep_ms(200);
        }

        sleep_ms(LOOP_INTERVAL_MS);
    }

    return 0;
}
