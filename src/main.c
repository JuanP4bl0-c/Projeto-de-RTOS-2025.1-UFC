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
#include <timers.h>
#include <task.h>
#include <semphr.h>
#include "hardware/adc.h"
#include "pico/stdlib.h"

/* 
Cabeçalhos customizados
*/ 

// #include "wifi.h" // O modulo Wi-Fi não está implementado e estamos com dificuldades de conexão com o serial da esp-01
#include "led.h"
#include "oled.h"
#include "servo_motor.h"

#define MEIA_HORA_MS (30 * 60 * 1000) 
#define SEC_timer (1000) 
#define MIN_timer (60 * 1000) 

#define ADC_Solo_Humidade 26 // GPIO26 (ADC0)
#define ADC_Exposição_solar 27 // GPIO27 (ADC1)
#define ADC_Temperatura 28 // GPIO28 (ADC2) - Não utilizado neste projeto, mas reservado para futuras expansões
#define ADC_MAX_VALUE 4095 // Valor máximo do ADC (12 bits)
#define VREF 3.3 // Tensão de referência do ADC

#define Motor_Servo 18 // GPIO18 (PWM0)
// Estrutura para configuração de cultivo
typedef struct {
    uint16_t humidade_ideal; // Humidade ideal em porcentagem
    uint16_t exposicao_solar_ideal; // Exposição solar ideal em porcentagem (exposição solar direta = 80%, indireta = 60%, sombra = 40%)
    uint8_t horas_registradas; // Exposição solar em horas
    const char *nome; // Nome do cultivo
    bool daytime; // Indica se o cultivo é diurno
    

} Cultivo_t;

// Estrutura para controle do sistema
typedef struct {
    TimerHandle_t timer; // Timer para checagem periódica
    TaskHandle_t task_handle; // Handle da tarefa de sensores
    SemaphoreHandle_t semaphore; // Semáforo para sincronização
    uint16_t ultima_humidade; // Última leitura de umidade
    uint16_t ultima_exposicao; // Última leitura de exposição solar
    uint16_t ultima_temperatura; // Última leitura de temperatura (reservado para futuras expansões)
} Sistema_t;

// Protótipos de funções
void sensor_task(void *pvParameters);
void timer_callback(TimerHandle_t xTimer);
void uint8_to_string(uint8_t value, char* buffer);
void print_ntc_voltage(void);

// Variáveis globais
Sistema_t sistema;
Cultivo_t cultivo = {65, 60, 6, "Alface", true};

void timer_callback(TimerHandle_t xTimer) {
    xSemaphoreGive(sistema.semaphore);
}

void sensor_task(void *pvParameters) {

    printf("Iniciando tarefa de leitura de sensores...\n");
    
    char display_line1[20];
    char display_line2[20];

    while(1) {
    
        xSemaphoreTake(sistema.semaphore, portMAX_DELAY);
        
        // Leituras dos sensores
        adc_select_input(0);
        sistema.ultima_humidade = adc_read();
        adc_select_input(1);
        sistema.ultima_exposicao = adc_read();
        
        // Processamento
        uint8_t humidade_percent = (sistema.ultima_humidade * 100) / ADC_MAX_VALUE;
        uint8_t exposicao_percent = (sistema.ultima_exposicao * 100) / ADC_MAX_VALUE;
        print_oled_stats(humidade_percent,exposicao_percent,cultivo.exposicao_solar_ideal,cultivo.nome); // Atualiza o OLED com as leituras
        
        //tratamento de umidade
        if(humidade_percent < cultivo.humidade_ideal) {
            printf("! ATIVANDO IRRIGAÇÃO !\n");
        }
        if (humidade_percent > (cultivo.humidade_ideal + 10)) {
            printf("! IRRIGAÇÃO SOBRE RISCO EXCESSO DE AGUA !\n");
        }

        //tratamento de exposição solar
        if(exposicao_percent < cultivo.exposicao_solar_ideal) {
            printf("! ATIVANDO MOTOR SERVO !\n");
            servo_set_angle(Motor_Servo, 90); // Ajusta o servo motor para 90 graus
        } else if (exposicao_percent > (cultivo.exposicao_solar_ideal + 10)) {
            printf("! MOTOR SERVO DESATIVADO !\n");
            servo_set_angle(Motor_Servo, 0); // Ajusta o servo motor para 0 graus
        } else {
            printf("! MOTOR SERVO MANTIDO NA POSIÇÃO !\n");
        }
        
    }
}

void tratamento(){
    // Função de tratamento de dados, se necessário
    // Pode ser usada para processar leituras adicionais ou realizar ações específicas
    printf("Tratamento de dados em execução...\n");
}

int main() {

    BlinkParams_t led0 = {LED_0, NULL, LED_Sample_Rate, "LED 0"}; // colocado no início para debbug
    
    // Inicialização de hardware
    adc_init();
    adc_gpio_init(ADC_Solo_Humidade);
    adc_gpio_init(ADC_Exposição_solar);

    servo_init(Motor_Servo);
    stdio_init_all();
    oled_init();

    oled_fill_screen(OLED_CLS); // Limpa a tela OLED

    // Criação do semáforo
    sistema.semaphore = xSemaphoreCreateBinary();
    configASSERT(sistema.semaphore != NULL);

    // Criação do timer
    sistema.timer = xTimerCreate("TimerChecagem",pdMS_TO_TICKS(SEC_timer*2),pdTRUE,NULL,timer_callback);
    xTimerStart(sistema.timer, 0);

    // Criação das tarefas
    xTaskCreate(sensor_task, "Sensor_Task", 256, NULL, 2, &sistema.task_handle);
    xTaskCreate(led_task, "LED_0", 256, &led0, 0, NULL);

    vTaskStartScheduler();

    for(;;);
}