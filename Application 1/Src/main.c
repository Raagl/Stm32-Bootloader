
/*Inside Sector 2*/

#include <stdint.h>
#include <stdio.h>
#include "stm32f411xe.h"
#include "fpu.h"
#include "uart.h"
#include "timebase.h"
#include "bsp.h"

#define GPIOAEN (1<<0)
#define PIN5	(1<<5)
#define LED_PIN PIN5


bool btn_state;

int main(void)
{

	// Enable FPU
	fpu_enable();

	// Initialize UART
	uart_debug_init();

	// Intialize timebase
	timebase_init();

	// Initialize led and button
	led_init();
	button_init();

	while(1)
	{
		printf("Application 1 started....\n\r");
	}
}

