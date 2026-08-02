/*
 * uart.h
 *
 *  Created on: 25 Jul 2026
 *      Author: Senthil
 */

#ifndef UART_H_
#define UART_H_

#include "stm32f411xe.h"
#define SR_RXNE (1U<<5)

void system_uart_init();
void uart_receive(uint8_t *buffer, uint32_t len);
void uart_transmit(uint8_t *buffer, uint32_t len);


#endif /* UART_H_ */
