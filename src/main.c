/*  

    Universidade Federal do Ceará - UFC
    Disciplina: Sistemas de tempo real - 2025.1
    Professor: FRANCISCO HELDER CANDIDO DOS SANTOS FILHO
    
    Alunos: JUAN PABLO RUFINO MESQUITA - 509982
            MONALISA SILVA BEZERRA - 535614
    
    email:  juanrufinmesq@alu.ufc.br 
            monasilva@alu.ufc.br

    Projeto: Sistema de Monitoramento e Controle de Cultivo de Alface
    Descrição: Este código implementa um sistema de monitoramento e controle para cultivo utilizando sensores de umidade e exposição solar.

*/

#include <FreeRTOS.h>
#include <task.h>
#include "hardware/adc.h"
#include "pico/stdlib.h"


/* 
Cabeçalhos customizados
*/ 

// #include "wifi.h" // O modulo Wi-Fi não está implementado e estamos com dificuldades de conexão com o serial da esp-01
#include "led.h"
#include "oled.h"
#include "servo_motor.h"
#include "Timer.h"
// #include "adc_custom.h"
#include "dht11.h"
#include "Bluethooth.h"

#define Queue_Size 4

// Estrutura para configuração de cultivo
typedef struct {
    uint8_t humidade_ideal; // Humidade ideal em porcentagem
    uint8_t exposicao_solar_ideal; // Exposição solar ideal em porcentagem (exposição solar direta = 80%, indireta = 60%, sombra = 40%)
    uint8_t temperatura_ideal; // Temperatura ideal em graus Celsius (reservado para futuras expansões)
    uint8_t horas_registradas; // Exposição solar em horas
    const char *nome; // Nome do cultivo
    bool daytime; // Indica se o cultivo é diurno

} Cultivo_t;

typedef struct {
    uint8_t hum_cent_quque;
    uint8_t sun_cent_queue;
    uint8_t temperatura_queue;
    uint8_t hr_queue;
} Data_cultivo_t;

QueueHandle_t data_Queue; // Fila para dados de sensores

void sensor_task(void *pvParameters);
void printout_task(void *pvParameters);
void tratamento_task(void *pvParameters);

#define ADC_Solo_Humidade 26 // GPIO26 (ADC0)
#define ADC_Exposição_solar 27 // GPIO27 (ADC1)

#define SLEEP_INTERVAL_MS HORA_MS
#define ADC_WAKE_THRESHOLD ((ADC_MAX_VALUE * 40) / 100) // 40%
#define ADC_MAX_VALUE 4095 // Valor máximo do ADC (12 bits)

#define porcentagem_REF 100 // Porcentagem de referência para cálculos

int main() {

    BlinkParams_t led0 = {LED_0, NULL, LED_Sample_Rate, "LED 0"}; // colocado no início para debbug
    Cultivo_t cultivo = {65, 70, 25, 6, "Alface", true};

    // Inicialização da fila de dados
    data_Queue = xQueueCreate(Queue_Size, sizeof(Data_cultivo_t));
    configASSERT(data_Queue != NULL);

    // Inicialização de hardware
    adc_init();
    adc_gpio_init(ADC_Solo_Humidade);
    adc_gpio_init(ADC_Exposição_solar);
    servo_init(SERVO_PIN); // Inicializa o servo motor
    Bluethooth_Setup();

    stdio_init_all();
    oled_init();

    oled_fill_screen(OLED_CLS); // Limpa a tela OLED

    
    // Criação do semáforo
    sistema.semaphore = xSemaphoreCreateBinary();
    configASSERT(sistema.semaphore != NULL);

    

    // Criação do timer
    sistema.timer = xTimerCreate("TimerChecagem",pdMS_TO_TICKS(SEC_timer*5),pdTRUE,NULL,timer_callback);
    xTimerStart(sistema.timer, 0);

    // Criação das tarefas
    xTaskCreate(led_task, "LED_0", 256, &led0, 3, NULL);
    xTaskCreate(sensor_task, "Sensor_Task", 256, &cultivo, 2, &sistema.task_handle);
    xTaskCreate(printout_task, "Print_data_Task", 256, &cultivo, 1, NULL); // task para exibir dados no OLED e no wifi/bluethooth
    xTaskCreate(tratamento_task, "Servo_Task", 256, &cultivo, 3, NULL);
    xTaskCreate(Task_Bluetooth_Receive, "Bluetooth_Receive", 256, NULL, 1, NULL);

    vTaskStartScheduler();

    for(;;);
}

void printout_task(void *pvParameters) {
    Cultivo_t cultivo = *(Cultivo_t *)pvParameters;
    Data_cultivo_t data;
    char buffer[4];
    while (1) {
        if (xQueueReceive(data_Queue, &data, portMAX_DELAY)) {
            
            print_oled_stats(data.hum_cent_quque, data.sun_cent_queue, cultivo.exposicao_solar_ideal,data.temperatura_queue, cultivo.nome);
            bluetooth_send("Dados do cultivo:\n");
            bluetooth_send("Humidade: ");
            uint8_to_string(data.hum_cent_quque, buffer);
            bluetooth_send(buffer);
            bluetooth_send("%%\n");
            bluetooth_send("Exposicao solar: ");
            uint8_to_string(data.sun_cent_queue, buffer);
            bluetooth_send(buffer);
            bluetooth_send("%%\n");
            bluetooth_send("\n");
            bluetooth_send("Temperatura: ");
            uint8_to_string(data.temperatura_queue, buffer);
            bluetooth_send(buffer);
            bluetooth_send("°C\n");
            bluetooth_send("Horas registradas: ");
            uint8_to_string(data.hr_queue, buffer);
            bluetooth_send(buffer);
            bluetooth_send("\n");
        }
    }
}

void tratamento_task(void *pvParameters) {

    Cultivo_t cultivo = *(Cultivo_t *)pvParameters;
    Data_cultivo_t data;
    uint8_t horas_restantes = cultivo.horas_registradas;
    uint8_t angulo_set = 0;

    servo_move_to_angle(SERVO_PIN, 0, 50); // Inicializa o servo motor na posição 0 graus

    for(;;) {

        if (xQueueReceive(data_Queue, &data, portMAX_DELAY)) {
            
            printf("angular atual: %d\n", angle_atual);

            //tratamento de umidade
            if(data.hum_cent_quque < cultivo.humidade_ideal) {
                printf("! ATIVANDO IRRIGAÇÃO !\n");
            }
            if (data.hum_cent_quque > (cultivo.humidade_ideal * 1.5)) {
                printf("! IRRIGAÇÃO SOBRE RISCO EXCESSO DE AGUA !\n");
            }

            //tratamento de temperatura            
            if (data.temperatura_queue > cultivo.temperatura_ideal*1.5) {
                printf("! TEMPERATURA ACIMA DO IDEAL !\n");
                angulo_set = 45;
            } else if (data.temperatura_queue < cultivo.temperatura_ideal) {
                printf("! TEMPERATURA ABAIXO DO IDEAL !\n");
                angulo_set = 75;
            }

            //tratamento de exposição solar
            if ((horas_restantes > 0) && cultivo.daytime)
            {
                if(data.sun_cent_queue < cultivo.exposicao_solar_ideal) {
                    printf("! ATIVANDO MOTOR SERVO !\n");
                    angulo_set = 90; // Ativa o servo motor para exposição solar ideal
                    
                } else if (data.sun_cent_queue > (cultivo.exposicao_solar_ideal + 1.5)) {
                    printf("! MOTOR SERVO DESATIVADO !\n");

                    angulo_set = 0; // Reseta o ângulo do servo motor

                } else {
                    
                    horas_restantes--; // Decrementa o contador de horas
                    printf("! MOTOR SERVO MANTIDO NA POSIÇÃO !\n");
                }

            }else
            {
                printf("Ciclo solar concluido\n");
                cultivo.daytime = false; // Muda o estado do cultivo para noturno
                oled_power_off(); // Desliga o OLED
                close_servo(SERVO_PIN); // Fecha o servo motor
                sleep_timer_init(); // Inicializa o timer de sleep
                /*
                o sistema entra em modo sleep por até 12 horas depois checa se existe exposição solar,
                Se houver, recomeça o ciclo de cultivo.
                Se não houver, o sistema permanece em sleep até que haja exposição solar novamente.
                */
            }

            servo_move_to_angle(SERVO_PIN, angulo_set, 50); // Move o servo motor para o ângulo desejado
            // Aguarda o intervalo de tempo definido
            vTaskDelay(pdMS_TO_TICKS(SEC_timer*5));

        }
    
    }
}

void sensor_task(void *pvParameters){
    
    Cultivo_t cultivo = *(Cultivo_t *)pvParameters;
    Data_cultivo_t data;
    uint8_t ambiente_humidade;

    while(1) {
        if(!cultivo.daytime){continue;} // Se não for dia, não executa a tarefa
        
        xSemaphoreTake(sistema.semaphore, portMAX_DELAY);
        
        // Leituras dos sensores
        adc_select_input(0);
        sistema.ultima_humidade = adc_read();
        adc_select_input(1);
        sistema.ultima_exposicao = adc_read();
        
        dht11_read(DHT_PIN,&sistema.ultima_temperatura,&ambiente_humidade);

        // Processamento
        uint8_t humidade_percent = porcentagem_REF - (sistema.ultima_humidade * porcentagem_REF) / ADC_MAX_VALUE;
        uint8_t exposicao_percent = (sistema.ultima_exposicao * porcentagem_REF) / ADC_MAX_VALUE;
        
        data.hum_cent_quque = humidade_percent;
        data.sun_cent_queue = exposicao_percent;
        data.temperatura_queue = sistema.ultima_temperatura;
        data.hr_queue = cultivo.horas_registradas;

        xQueueSend(data_Queue, &data, 0); // Envia os dados para a fila    
        xQueueSend(data_Queue, &data, 0); // Envia os dados para a fila    
        
    }
}