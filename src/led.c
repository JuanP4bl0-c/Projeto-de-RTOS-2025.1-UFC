#include "led.h"

void intro_task(void *pvParameters) {
    
    gpio_init(LED_1);
    gpio_set_dir(LED_1, GPIO_OUT);
    
    int timer = 3;
    const int delay = 25;
    while(timer) {
        gpio_put(LED_1, 1);
        vTaskDelay(delay);
        gpio_put(LED_1, 0);
        vTaskDelay(delay*100);
        timer--;
    }
    printf("Intro concluída\n");
    vTaskDelete( NULL );
}

void led_task(void *pvParameters) //parametro utiliza  da strucct BlinkParams_t
{   
    
    BlinkParams_t *params = (BlinkParams_t *)pvParameters;
    
    gpio_init(params->led_pin);
    gpio_set_dir(params->led_pin , GPIO_OUT);
    
    while(true) {
        gpio_put(params->led_pin, ON);
        params->info = ON;
        vTaskDelay(params->delay_ms / portTICK_PERIOD_MS);
        
        gpio_put(params->led_pin, OFF);
        params->info = OFF;
        vTaskDelay(params->delay_ms / portTICK_PERIOD_MS);
    }
    
    vTaskDelete( NULL );
}