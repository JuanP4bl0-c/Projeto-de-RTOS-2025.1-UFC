#include <FreeRTOS.h>
#include <task.h>

//Cabecalhos customizados
#include "led.h"
#include "Bluethooth.h"
#include "oled.h"

int main() {
    
    BlinkParams_t led0 = {LED_0,NULL, LED_Sample_Rate, "LED 0"};
    
    //Inicialização do hardware
    oled_init();
    
    oled_fill_screen();
    oled_write_string(0, 0, "1234567890");
    oled_write_string(0, 1, "PICO + FREERTOS");
    Bluethooth_Setup();


    stdio_init_all();

    xTaskCreate(led_task, "LED_0", 256, &led0, 4, NULL);
    xTaskCreate(Task_Bluetooth, "BT_send", 256, NULL, 3, NULL);
    xTaskCreate(Task_Bluetooth_Receive, "BT_receive", 256, NULL, 3, NULL);
    

    vTaskStartScheduler();

    while(1);
}