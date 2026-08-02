#include "bootloader.h"

typedef void(*func_ptr)(void);


/*Bootloader*/
void jump_to_app (uint32_t app_address)
{
	uint32_t app_start_address;
	func_ptr jump_to_app;

	printf("Bootloader started..... \n\r");
	delay(300);

#ifdef MEMCHECK_V1
	if ((*(uint32_t*)app_address)!= EMPTY_VALUE)
#endif
#ifdef MEMCHECK_V2
	if((*(uint32_t*)app_address) & MSP_VERIFY_MASK == 0x20020000)
#endif

	{
		printf("Starting Application.....\n\r");
		app_start_address = *(uint32_t *)(app_address + 4);

		jump_to_app = (func_ptr)(app_start_address);

		/*De-initialize Bootloader*/
		bootloader_deinit();

        /* Set Vector Table to Application */
        SCB->VTOR = app_address;

		/*Initialialize the main stack pointer*/
		__set_MSP(*(uint32_t*)app_address);

		/*jump*/
		jump_to_app();
	}
	else
	{
		printf("No Application Found...\n\r");
	}
}

void bootloader_init(void)
{
	// Enable FPU
	fpu_enable();

	// Initialise UART
	system_uart_init();

	// Initialise timebase
	timebase_init();

	// Initialise led button
	led_init();

	// Initialise button
	button_init();
}

void bootloader_deinit(void)
{
    /* Disable global interrupts */
    __disable_irq();

    /* Disable SysTick */
    SysTick->CTRL = 0;
    SysTick->LOAD = 0;
    SysTick->VAL  = 0;

    /* Disable UART2 */
    USART2->CR1 = 0;
    USART2->CR2 = 0;
    USART2->CR3 = 0;

    /* Clear and disable all NVIC interrupts */
    for(uint8_t i = 0; i < 8; i++)
    {
        NVIC->ICER[i] = 0xFFFFFFFF;
        NVIC->ICPR[i] = 0xFFFFFFFF;
    }
}
