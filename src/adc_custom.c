#include "adc_custom.h"

QueueHandle_t ADC_queue;
SemaphoreHandle_t Printf_mutex;

void ADC_Read_Task(void *pvParameters){
    ADC_queue = xQueueCreate(Queue_Lenght,sizeof(uint16_t));
    adc_init();
    adc_gpio_init(ADC_PIN);
    adc_select_input(ADC_INPUT);

    for (;;)
    {
        uint16_t val = adc_read();
        float voltagem = (val / ADC_RESOLUCAO) * ADC_VOLT_FER; 

        printf("ADC Val: %d \n ",val);
        //printf("Voltagem: %.2f \n ",voltagem);

        xQueueSend(ADC_queue,&val,portMAX_DELAY);

        vTaskDelay(ADC_DELAY);
    }
    
}

void RGB_Led_task(void *pvParameters){

    gpio_init(RBG_PIN_R);
    gpio_init(RBG_PIN_G);
    gpio_init(RBG_PIN_B);

    gpio_set_dir(RBG_PIN_R, GPIO_OUT);
    gpio_set_dir(RBG_PIN_G, GPIO_OUT);
    gpio_set_dir(RBG_PIN_B, GPIO_OUT);
    

    uint16_t valor = 0;
    for (;;)
    {
        if (xQueueReceive(ADC_queue, &valor, portMAX_DELAY) == pdPASS)
        {
            if (valor > 0 && valor <= 1000)
            {
                gpio_put(RBG_PIN_R, OFF);
                gpio_put(RBG_PIN_G, OFF);
                gpio_put(RBG_PIN_B, ON);
            }else if(valor > 1000 && valor <= 2400){
                gpio_put(RBG_PIN_R, OFF);
                gpio_put(RBG_PIN_G, ON);
                gpio_put(RBG_PIN_B, OFF);
            }else if(valor > 2400){
                gpio_put(RBG_PIN_R, ON);
                gpio_put(RBG_PIN_G, OFF);
                gpio_put(RBG_PIN_B, OFF);
            }else{
                gpio_put(RBG_PIN_R, OFF);
                gpio_put(RBG_PIN_G, OFF);
                gpio_put(RBG_PIN_B, OFF);
            }
            
        }
        vTaskDelay(ADC_DELAY);
    }
    
}

void acelerometro_read_task(void *pvParameters) {
    Acelerometro_Params_t *params = (Acelerometro_Params_t *)pvParameters;
    for(;;){

        printf("Leitura do acelerometro sendo executada.. \n");

        if (xSemaphoreTake(Printf_mutex, params->deadline_ms / portTICK_PERIOD_MS)){
            params->valor = adc_read();
            xSemaphoreGive(Printf_mutex);
        }else
        {
            printf("Acelerometro read timeout: %d ms\n\n", params->deadline_ms);
            params->valor = 0; // Reset value on timeout
        }
        

        vTaskDelay(pdMS_TO_TICKS(params->delay_ms));
    
    }
}


void acelerometro_print_task(void *pvParameters) {
    Acelerometro_Params_t *params = (Acelerometro_Params_t *)pvParameters;
    
    for(;;){
        if (xSemaphoreTake(Printf_mutex, params->deadline_ms / portTICK_PERIOD_MS)){
            
            printf("Acelerometro valor: %d\n", params->valor);
            params->valor = 0; // Reset value after printing

            xSemaphoreGive(Printf_mutex);
        }else{
            printf("Acelerometro print timeout: %d ms\n\n", params->deadline_ms);
        }
        vTaskDelay(pdMS_TO_TICKS(params->delay_ms));
    }
}

void acelerometro_volt_print_convert_task(void *pvParameters){
    Acelerometro_Params_t *params = (Acelerometro_Params_t *)pvParameters;
    for(;;){
        if (xSemaphoreTake(Printf_mutex, params->deadline_ms / portTICK_PERIOD_MS)){
            
            uint32_t voltage = (params->valor * Voltage_ref * Voltage_mant) / (1<<12);
            uint8_t voltage_decimal = voltage % Voltage_mant;
            voltage /= Voltage_mant;

            printf("Voltagem : %d.%.2dV \n",voltage,voltage_decimal);
            // /vTaskDelay(task_deadline_rate_max); //delay para exeder o deadline.
            xSemaphoreGive(Printf_mutex);
        }else{
            printf("Acelerometro print voltage timeout: %d ms\n\n", params->deadline_ms);
        }
        vTaskDelay(pdMS_TO_TICKS(params->delay_ms));
        
    }
}