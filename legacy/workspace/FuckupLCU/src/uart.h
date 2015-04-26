/**
 * comm.h
 *
 * This file defines the common communication protocol between the Fuckup LLCU and HLCU.
 *
 *  Created on: Mar 19, 2013
 *      Author: PETERSEN
 */

#ifndef UART_H_
#define UART_H_

#include "stm32f30x.h"

#define UART_BAUDRATE		115200			// set baudrate to 115200 bit/sec

#define UART_SENDQUEUE_SIZE	100				// set queue size for about 10 messages


typedef enum {
	UART_Rx,
	UART_Tx
} uart_module_t;



void uart_init();

void uart_enable(uart_module_t module);
void uart_enable_both(void);

void uart_disable(uart_module_t module);
void uart_disable_both(void);

void uart_send(uint8_t head, uint8_t size, uint8_t *data);



#endif /* UART_H_ */
