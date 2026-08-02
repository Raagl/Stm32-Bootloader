/*
 * bsp.c
 *
 *  Created on: 26 Jul 2026
 *      Author: Senthil
 */


#include "bsp.h"

#define GPIOAEN 		(1U<<0)
#define GPIOCEN 		(1U<<2)

#define LED_PIN 		(1U<<5)
#define BUTTON_PIN 		(1U<<13)


void led_init(void)
{
	// Enable clock access to GPIOA in AHB1 bus
	RCC->AHB1ENR |= GPIOAEN;

	// Set PA5 to output mode
	GPIOA->MODER |= (1U<<10);
	GPIOA->MODER &=~ (1U<<11);
}

void led_on(void)
{
	GPIOA->ODR |= (1U<<5);
}

void led_off(void)
{
	GPIOA->ODR &=~ (1U<<5);
}

void button_init(void)
{
	// Enable clock access to GPIOC in AHB1 bus
	RCC->AHB1ENR |= GPIOCEN;

	// Set PC13 to input mode
	GPIOC->MODER &=~ (1u<<26);
	GPIOC->MODER &=~ (1u<<27);
}

bool get_button_state(void)
{
	// ACTIVE LOW
	if(GPIOC->IDR & BUTTON_PIN)
	{
		return false;
	}
	else
	{
		return true;
	}
}

