/*
 * dht11.c
 *
 *  Created on: Sep 23, 2024
 *      Author: juan
 */

#include "stm32f1xx.h"
#include <stdint.h>
#include "dht11.h"
#include "tim2.h"

#include "Usart1.h"

//dht11
#define DHT11_PIN 	0x200
#define DHT11_GRUPO GPIOB

void dht11_setup(){

	RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;

	DHT11_GRUPO->CRH &= ~(GPIO_CRH_MODE9 | GPIO_CRH_CNF9);
	DHT11_GRUPO->CRH |= GPIO_CRH_MODE9_1;

    DHT11_GRUPO->ODR &= ~DHT11_PIN;  // Pino em LOW

}

void dht11_start(){

	dht11_setup();

	DHT11_GRUPO->ODR &= ~DHT11_PIN;
	TIM2_delay_ms(18);

	DHT11_GRUPO->ODR |= DHT11_PIN;
	TIM2_delay_us(30);

}

uint8_t dht11_ping() {

	dht11_start();


    uint8_t sinal = 0;

    DHT11_GRUPO->CRH &= ~(GPIO_CRH_MODE9 | GPIO_CRH_CNF9);
    DHT11_GRUPO->CRH |= GPIO_CRH_CNF9_0; //float input

    // Esperar o DHT11 puxar o pino para baixo
    TIM2_delay_us(40);

    if (!(DHT11_GRUPO->IDR & DHT11_PIN)){
    	//uart_write("pong!\n\r");
    	TIM2_delay_us(80);
        if (DHT11_GRUPO->IDR & DHT11_PIN) {
            sinal = 1; // DHT11 respondeu
            //uart_write("pong!\n\r");
        }
    }

    // Esperar o fim da resposta
    while (DHT11_GRUPO->IDR & DHT11_PIN);
    //uart_write("\n\r ping encerrado\n\r");
    return sinal;
}




uint8_t dht11_read(){
	uint8_t i, result = 0;

    for (i = 0; i < 8; i++) {

        // Esperar o pino ficar baixo
        while (!(DHT11_GRUPO->IDR & DHT11_PIN));

        // Esperar 40 us e verificar o estado do pino
        TIM2_delay_us(40);
        if (DHT11_GRUPO->IDR & DHT11_PIN) {
            result |= (1 << (7 - i)); // Se o pino estiver alto, é 1
        }

        // Esperar o pino ficar baixo de novo
        while (DHT11_GRUPO->IDR & DHT11_PIN);
    }

    return result;
}
