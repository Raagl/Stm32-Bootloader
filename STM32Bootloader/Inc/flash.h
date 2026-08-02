#ifndef FLASH_H_
#define FLASH_H_

#include "stm32f411xe.h"
#include <stdint.h>

#define OPTKEY1 0x08192A3B
#define OPTKEY2 0x4C5D6E7F

#define FKEY1	0x45670123
#define FKEY2	0xCDEF89AB

#define FLASH_MAX_SECTOR      (7U)
#define FLASH_MASS_ERASE      (0xFFU)
#define FLASH_OK              (0x00U)
#define FLASH_FAIL            (0x01U)
#define FLASH_INVALID_SECTOR  (0x04U)

#define FLASH_SR_OPERR		(1U<<1)
#define FLASH_OPTCR_SPRMOD	(1U<<31);



uint8_t flash_erase(uint8_t sector_number , uint8_t number_of_sector);
uint8_t flash_program(uint32_t mem_address, uint8_t payload_length, uint8_t *data);
uint8_t flash_enable_rw_protection(uint8_t sector_mask, uint8_t protection_mode);
uint8_t flash_disable_rw_protection(void);
uint8_t flash_get_sector_status(void);


void flash_lock(void);
void flash_unlock(void);

#endif /* FLASH_H_ */
