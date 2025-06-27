#include "bttn.h"

void gpio_callback(uint gpio, uint32_t events) {
    
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    xSemaphoreGiveFromISR(xButtonSemaphore, &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

void Button_task(void *pvParameters) {
    
    Bttn_Params_t *params = (Bttn_Params_t *)pvParameters;
    uint32_t state = ON;
    
    gpio_init(params->pin);
    gpio_set_dir(params->pin, GPIO_IN);
    gpio_pull_up(params->pin); 
    
    gpio_set_irq_enabled(params->pin, GPIO_IRQ_EDGE_RISE, true);
    
    for (;;) {
        
        if (xSemaphoreTake(xButtonSemaphore, portMAX_DELAY) == pdTRUE) {
            
            vTaskDelay(pdMS_TO_TICKS(params->debouce_delay));
            if (gpio_get(params->pin) == 1) { // Verifica se o botão está pressionado
                xQueueSend(BTTN_Queue, &state, 0);
            }       
        }

        //vTaskDelay(BTTN_DELAY);
    }
    
    vTaskDelete(NULL);
}

void process_button_task(void *pvParameters) {

    BlinkParams_t *params = (BlinkParams_t *)pvParameters;

    gpio_init(params->led_pin);
    gpio_set_dir(params->led_pin , GPIO_OUT);

    uint32_t received_state;
    uint8_t togglte_state = 0;

    while (true) {

        if (xQueueReceive(BTTN_Queue, &received_state, portMAX_DELAY) == pdPASS) {
            if (received_state) {
                togglte_state = !togglte_state; // Toggle the state
                gpio_put(params->led_pin, togglte_state);
                vTaskDelay(params->delay_ms / portTICK_PERIOD_MS);
            }
        }
    }
}


int is_semaphore_available(SemaphoreHandle_t sem) {
    return uxSemaphoreGetCount(sem) > 0;
}
void semaforo_check(void* pvParameters) {
    SemaphoreHandle_t sem = (SemaphoreHandle_t)pvParameters;
    for (;;)
    {
        if (sem == NULL) {
            printf("Erro: Semáforo não inicializado.\n");
        } else if (is_semaphore_available(sem)) {
            printf("Semáforo está disponível.       \r");
        } else {
            printf("Semáforo não está disponível.   \r");
        }
        vTaskDelay(pdMS_TO_TICKS(BTTN_DELAY)); // Verifica a cada segundo
    }
}


