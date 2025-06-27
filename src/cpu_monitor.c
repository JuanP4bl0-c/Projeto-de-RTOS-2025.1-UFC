#include "cpu_monitor.h"

static unsigned long int idle_tick_counter = 0UL;
static char pcWriteBuffer[512];

uint8_t Handler_cnt = 0;
TaskHandle_t	pTaskParams[MAX_FREERTOS_TASKS];  // every FreeRTOS task gets an entry in this array.

TaskHandle_t *set_TaskHandler_Idex(void *pvParameters){

    if (Handler_cnt >= 0 && Handler_cnt < MAX_FREERTOS_TASKS) {
        
        Handler_cnt++;
        return &pTaskParams[(Handler_cnt) - 1];  // Retorna o endereço do TaskHandle no índice especificado
        
    }
    return NULL; 
}

char* get_state_task(TaskHandle_t handler_task){
    eTaskState stats = eTaskGetState(handler_task);
    switch (stats)
    { 
    case eRunning:
        return "Executando";
    break;
        
    case eReady:
        return "Ready";
    break;
        
    case eBlocked:
        return "Blocked";
    break;
        
    case eSuspended:
        return "suspended";
        break;
        
        case eDeleted:
        return "apagado";
        break;
    
    default:
        return "Desconhecido";
    break;
    }  
}

void vApplicationIdleHook(void){
    unsigned long int tick = xTaskGetTickCount();
    while (xTaskGetTickCount() == tick);
    idle_tick_counter++;
}

void cpu_measure_task(void *pvParameters) {
    unsigned long int idle_tick_last = 0, ticks;
	while(1){
        // wait 3 seconds
		vTaskDelay(3000/portTICK_PERIOD_MS);
        
        
		if (idle_tick_counter > idle_tick_last){
            ticks = idle_tick_counter - idle_tick_last;
        }
		else{
            ticks = 0xFFFFFFFF - idle_tick_last + idle_tick_counter;
        }
        
		ticks /= uxTaskGetNumberOfTasks(); //num de tasks definidas
        
		/* Printing idle ticks per second */
		printf("%ld idle ticks per second (out of %ld) - %i tasks - ", ticks, configTICK_RATE_HZ, uxTaskGetNumberOfTasks());
        
		/* calc and print CPU usage */
		ticks = (configTICK_RATE_HZ - ticks)/10;
		printf("CPU usage: %d%% \n", ticks);
        
		idle_tick_last = idle_tick_counter;
        
        
        //addons
        printf("======================================\n");
        
        memset(pcWriteBuffer, 0, sizeof(pcWriteBuffer));
        
        // Obtém estatísticas formatadas
        vTaskGetRunTimeStats(pcWriteBuffer);
        // Limpa console e exibe estatísticas
        printf("Nome =========== Ticks ======= Consumo\n");
        printf("%s", pcWriteBuffer);
        printf("======Prioridades=======Estado========\n");
        for (size_t i = 0; i < uxTaskGetNumberOfTasks() - 1; i++)
        {
            UBaseType_t type_priority =uxTaskPriorityGet(pTaskParams[i]); 
            char* type_name = pcTaskGetName(pTaskParams[i]); 

            printf("%s - %u - %s \n",type_name ,type_priority,get_state_task(pTaskParams[i]));
        }
	}



    vTaskDelete( NULL );
}