/*
 * dht11.h
 *
 *  Created on: Sep 23, 2024
 *      Author: juan
 */

#ifndef DHT11_H_
#define DHT11_H_

#include <stdint.h>

void dht11_setup();

void dht11_start();

uint8_t dht11_ping();

uint8_t dht11_read();


#endif /* DHT11_BARE_METAL_CUSTOM_LIB_H_ */
