/*Multi Slot Bootloader*/

#include "main.h"


int main(void)
{
	/*Initialise Hardware*/
	bootloader_init();

    /* If button is not pressed, boot immediately */
    if (!get_button_state())
    {
        printf("Booting Default Application...\r\n");
        jump_to_app(DEFAULT_APPLICATION_ADDRESS);
    }
    else
	{
    	/*Display Boot option*/
    	led_on();
    	bootloader_uart_mode();
	}

	while(1)
	{
	}
}

