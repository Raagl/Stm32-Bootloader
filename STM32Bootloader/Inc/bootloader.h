#ifndef BOOTLOADER_H_
#define BOOTLOADER_H_

#include <stdint.h>
#include <stdio.h>
#include "stm32f411xe.h"

#include "fpu.h"
#include "uart.h"
#include "timebase.h"
#include "bsp.h"

#define EMPTY_VALUE 		0xFFFFFFFF
#define MSP_VERIFY_MASK		0x2FFE0000
#define MEMCHECK_V1


void jump_to_app (uint32_t app_address);
void bootloader_init(void);
void bootloader_deinit(void);


#endif /* BOOTLOADER_H_ */
