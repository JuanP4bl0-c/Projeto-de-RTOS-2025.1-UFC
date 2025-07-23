#include "dht11.h"

#define DHT_MAX_TIMINGS 85



bool dht11_read(uint gpio, uint8_t *temperature, uint8_t *humidity) {
    int data[5] = {0,0,0,0,0};
    int bitidx = 0, counter = 0, laststate = 1, j = 0;

    gpio_init(gpio);
    gpio_set_dir(gpio, GPIO_OUT);
    gpio_put(gpio, 0);
    sleep_ms(18);
    gpio_put(gpio, 1);
    sleep_us(40);
    gpio_set_dir(gpio, GPIO_IN);

    for (int i = 0; i < DHT_MAX_TIMINGS; i++) {
        counter = 0;
        while (gpio_get(gpio) == laststate) {
            counter++;
            sleep_us(1);
            if (counter == 255) break;
        }
        laststate = gpio_get(gpio);
        if (counter == 255) break;
        if ((i >= 4) && (i % 2 == 0)) {
            data[j/8] <<= 1;
            if (counter > 40) data[j/8] |= 1;
            j++;
        }
    }

    if ((j >= 40) &&
        (data[4] == ((data[0] + data[1] + data[2] + data[3]) & 0xFF))) {
        *humidity = data[0];
        *temperature = data[2];
        return true;
    } else {
        return false;
    }
}

void oled_write_uint8(uint8_t x, uint8_t page, uint8_t value, const char *label) {
    char buffer[20];
    if (label)
        sprintf(buffer, "%s: %u", label, value);
    else
        sprintf(buffer, "%u", value);
    oled_write_string(x, page, buffer);
}

void dht11_task(void *params) {
    dht11_params_t *dht_data = (dht11_params_t *)params;
    
    uint8_t temp, hum;
    while (1) {
        if (dht11_read(dht_data->led_pin , &temp, &hum)) { // GPIO2, ajuste conforme seu hardware
            dht_data->temperatura = temp;
            dht_data->humidade = hum;
            oled_write_uint8(0, 6, dht_data->temperatura, "Temperatura:");
            printf("Temperatura: %dC\n", dht_data->temperatura);
        }
        vTaskDelay(pdMS_TO_TICKS(2000)); // Aguarda 2 segundos
    }
}