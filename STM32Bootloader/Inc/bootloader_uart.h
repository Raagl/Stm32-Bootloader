
#ifndef BOOTLOADER_UART_H_
#define BOOTLOADER_UART_H_

#include <stdio.h>
#include <string.h>
#include "stm32f411xe.h"
#include "timebase.h"
#include "uart.h"
#include "crc.h"
#include "error.h"
#include "bsp.h"
#include "bootloader.h"
#include "flash.h"

#define BL_RX_BUFFER_SIZE 		256
#define BL_ACK			  		0xA5
#define BL_NACK			  		0x7F

#define BL_GET_VER_CMD 					0x51
#define BL_GET_HELP_CMD					0x52
#define BL_GET_CID_CMD					0x53
#define BL_GET_RDP_STATUS_CMD			0x54
#define BL_GO_TO_ADDR_CMD				0x55
#define BL_FLASH_ERASE_CMD 				0x56
#define BL_MEM_WRITE_CMD 				0x57
#define BL_EN_R_W_PROTECT_CMD			0X58
#define BL_READ_SECTOR_P_STATUS_CMD		0x5A
#define BL_DIS_R_W_PROTECT_CMD			0x5C



#define BL_VERSION				0x10U
#define ADDR_VALID				0x01
#define ADDR_INVALID			0x00


typedef enum
{
	BL_GET_VER 				= BL_GET_VER_CMD,
	BL_GET_HELP				= BL_GET_HELP_CMD,
	BL_GET_CID				= BL_GET_CID_CMD,
	BL_GET_RDP_STATUS		= BL_GET_RDP_STATUS_CMD,
	BL_GO_TO_ADDR			= BL_GO_TO_ADDR_CMD,
	BL_FLASH_ERASE  		= BL_FLASH_ERASE_CMD,
	BL_MEM_WRITE    		= BL_MEM_WRITE_CMD,
	BL_EN_R_W_PROTECT   	= BL_EN_R_W_PROTECT_CMD,
	BL_READ_SECTOR_P_STATUS	= BL_READ_SECTOR_P_STATUS_CMD,
	BL_DIS_R_W_PROTECT		= BL_DIS_R_W_PROTECT_CMD,
}bl_cmd_e;

#define SRAM1_END 0x20020000UL


void bootloader_uart_mode(void);

#endif /* BOOTLOADER_UART_H_ */
