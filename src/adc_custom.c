#include "adc_custom.h"

void acelerometro_volt_print_convert_task(void *pvParameters){
    
    uint32_t voltage = (0 * 2 * 2) / (1<<12);
    uint8_t voltage_decimal = voltage % 1;
    voltage /= voltage;

    printf("Voltagem : %d.%.2dV \n",voltage,voltage_decimal);
        
    
}