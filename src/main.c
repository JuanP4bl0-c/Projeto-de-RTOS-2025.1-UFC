#include <FreeRTOS.h>
#include <task.h>

#include "hardware/adc.h"
#include "pico/stdlib.h"

//Cabecalhos customizados
#include "led.h"
#include "oled.h"
#include "wifi.h" // TODO: substituição do ESP8266 por um módulo Bluetooth
#include "servo_motor.h"

#define ADC_Solo_Humidade 26 // GPIO26 (ADC0)
#define ADC_Exposição_solar 27 // GPIO27 (ADC1)

// Lê o valor do sensor de umidade de solo (0~4095)
uint16_t soil_sensor_read(uint adc_channel) {
    adc_select_input(adc_channel); // Ex: 0 para GPIO26
    return adc_read();
}

uint16_t sun_sensor_read(uint adc_channel) {
    adc_select_input(adc_channel); // Ex: 1 para GPIO27
    return adc_read();
}

void sensores_reading_task(void *pvParameters) {
    
    //inicalização do ADC da leitura do sensor de umidade do solo
    adc_init();
    adc_gpio_init(ADC_Solo_Humidade);

    while (true) {
        uint16_t valor_soil = soil_sensor_read(0); // Canal 0
        printf("Valor do sensor de umidade do solo: %d\n", valor_soil);
        
        uint16_t valor_sun = sun_sensor_read(1); // Canal 0
        printf("Valor do sensor de luz: %d\n", valor_sun);

        vTaskDelay(pdMS_TO_TICKS(1000)); // Espera 1 segundo
    }
}


int main() {
    
    BlinkParams_t led0 = {LED_0,NULL, LED_Sample_Rate, "LED 0"};
    
    //Inicialização do hardware
    // esp8266_send_cmd("AT+RST"); // Reinicia o módulo ESP8266
    
    stdio_init_all();

    //xTaskCreate(led_task, "LED_0", 256, &led0, 2, NULL);
    // xTaskCreate(esp8266_task, "ESP8266", 256, NULL, 4, NULL);
    xTaskCreate(esp8266_ap_webserver_task, "ESP8266_AP", 256, NULL, 4, NULL);
    vTaskStartScheduler();

    while(1);
}