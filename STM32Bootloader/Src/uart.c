/*
 * uart.c
 *
 *  Created on: 25 Jul 2026
 *      Author: Senthil
 */

#include <stdint.h>
#include "uart.h"

#define GPIOAEN (1<<0)
#define UART2EN (1U<<17)

#define DBG_UART_BAUDRATE 	115200
#define SYS_FREQ		 	16000000
#define APB1_CLK			SYS_FREQ
#define CR1_TE				(1U<<3)
#define CR1_RE				(1U<<2)

#define CR1_UE				(1U<<13)
#define CR1_RXNEIE			(1U<<5)
#define SR_TXE				(1U<<7)
#define SR_TC			    (1U<<6)

static void uart_set_baudrate(uint32_t periph_clk, uint32_t baudrate);
static void uart_write(int ch);

int __io_putchar(int ch)
{
	uart_write(ch);
	return ch;
}

void system_uart_init()
{
	// Enable clock access to APB1->GPIOA
	RCC->AHB1ENR |= GPIOAEN;

	//Set GPIOA PA2,PA3 to AF mode
	GPIOA->MODER |= (1<<5);
	GPIOA->MODER &=~ (1<<4);

	GPIOA->MODER |= (1<<7);
	GPIOA->MODER &=~ (1<<6);


	// GPIOA set PA2,PA3 -> AF7
	GPIOA->AFR[0] &=~ (1<<11);
	GPIOA->AFR[0] |= (1<<10);
	GPIOA->AFR[0] |= (1<<9);
	GPIOA->AFR[0] |= (1<<8);

	GPIOA->AFR[0] &=~ (1<<15);
	GPIOA->AFR[0] |= (1<<14);
	GPIOA->AFR[0] |= (1<<13);
	GPIOA->AFR[0] |= (1<<12);

	// ENABLE CLOCK ACESSS TO UART2
	RCC->APB1ENR |= UART2EN;

	//Configure the baud rate
	uart_set_baudrate(APB1_CLK, DBG_UART_BAUDRATE);

	// Set TX and RX
	USART2->CR1 = (CR1_TE | CR1_RE);

//	// Enable RXNEIE interrupt
//	USART2->CR1 |= CR1_RXNEIE;
//
//	// Enable NVIC
//	NVIC_EnableIRQ(USART2_IRQn);

	// ENABLE UART2
	USART2->CR1 |= CR1_UE;

}

void uart_transmit(uint8_t *buffer, uint32_t len)
{
	while(len--)
	{
		while(!(USART2->SR & SR_TXE)){}

		USART2->DR = *buffer++;
	}

	while(!(USART2->SR & SR_TC));
}

void uart_receive(uint8_t *buffer, uint32_t len)
{
	while(len--)
	{
		while(!(USART2->SR & SR_RXNE)){}
		*buffer++ = USART2->DR;
	}
}

void uart_write(int ch)
{
	// Make sure the transmit register is empty
	while(!(USART2->SR & SR_TXE)){}

	// Transmit the data
	USART2->DR = (ch & 0xFF);
}

static uint16_t compute_uart_bd(uint32_t periph_clk, uint32_t baudrate)
{
	return((periph_clk + (baudrate/2U)) /baudrate);
}

static void uart_set_baudrate(uint32_t periph_clk, uint32_t baudrate)
{
	USART2->BRR = compute_uart_bd(periph_clk, baudrate);
}
