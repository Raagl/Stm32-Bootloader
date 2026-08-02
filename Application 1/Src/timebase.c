/*
 * timebase.c
 *
 *  Created on: 26 Jul 2026
 *      Author: Senthil
 */


#include "timebase.h"

#define ONE_SEC_LOAD		 	16000000
#define ONE_MS_LOAD				16000

#define CTRL_CLOCKEN			(1<<0)
#define CTRL_TICKINT			(1<<1)
#define CTRL_CLOCKSRC			(1<<2)
#define CTRL_COUNTFLAG			(1<<16)

#define TICK_FREQ				1
#define MAX_DELAY       		0xFFFFFFFF
volatile uint32_t g_curr_tick;
volatile uint32_t g_curr_tick_p;


void delay(uint32_t delay)
{
	uint32_t start_tick = get_tick();
	uint32_t wait = delay;
	if(delay < MAX_DELAY)
	{
		wait += (uint32_t)TICK_FREQ;
	}

	while((get_tick() - start_tick)<wait){}

}

uint32_t get_tick(void)
{
	__disable_irq();
	g_curr_tick_p = g_curr_tick;
	__enable_irq();
	return g_curr_tick_p;
}

static void tick_increment(void)
{
	g_curr_tick += TICK_FREQ;
}


void timebase_init(void)
{
	// Disable global interrupt
	__disable_irq();

	// Load the no of clock per second
	SysTick->LOAD = ONE_MS_LOAD;

	// Clear the current count
	SysTick->CTRL = 0;

	// Select the clock source
	SysTick->CTRL |= CTRL_CLOCKSRC;

	// Enable the interrupt
	SysTick->CTRL |= CTRL_TICKINT;

	//  Enable systic timer
	SysTick->CTRL |= CTRL_CLOCKEN;

	// Enable global interrupt
	__enable_irq();
}


void SysTick_Handler(void)
{
	tick_increment();
}
