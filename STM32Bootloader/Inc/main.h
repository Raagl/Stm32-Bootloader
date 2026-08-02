#ifndef MAIN_H_
#define MAIN_H_

#include <stdint.h>
#include <stdio.h>

#include "stm32f411xe.h"
#include "bsp.h"
#include "bootloader.h"
#include "bootloader_uart.h"

#define GPIOAEN (1<<0)
#define PIN5	(1<<5)
#define LED_PIN PIN5

volatile char g_ch_key;
volatile uint8_t g_un_key = 0xFF;


#define MEMORY_SECTOR1_BASE_ADDRESS		FLASH_BASE | 0x4000
#define MEMORY_SECTOR2_BASE_ADDRESS 	FLASH_BASE | 0x8000
#define MEMORY_SECTOR3_BASE_ADDRESS 	FLASH_BASE | 0xC000


#define DEFAULT_APPLICATION_ADDRESS 	MEMORY_SECTOR1_BASE_ADDRESS


bool btn_state;


typedef void(*func_ptr)(void);


#endif /* MAIN_H_ */
