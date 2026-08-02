
#include "flash.h"

static void flash_wait_busy(void);
static uint8_t flash_sector_erase(uint8_t sector);
static uint8_t flash_mass_erase(void);
static uint8_t flash_error_check(void);
static uint8_t flash_program_error_check(void);
static void flash_option_lock(void);
static void flash_option_unlock(void);


uint8_t flash_erase(uint8_t sector_number , uint8_t number_of_sector)
{
	uint8_t status = FLASH_OK;

	/*Mass erase*/
	if(sector_number == FLASH_MASS_ERASE)
	{
		status = flash_mass_erase();

		return status;
	}

	/*Check if the sector number and number of sector is valid*/
	if ((sector_number + number_of_sector - 1) > FLASH_MAX_SECTOR  || number_of_sector ==0)
	{
	    return FLASH_INVALID_SECTOR;
	}

	/*unlock the flash*/
	flash_unlock();

	for (uint8_t i=0; i<number_of_sector; i++)
	{
		status = flash_sector_erase(sector_number+i);
		if(status != FLASH_OK) break;
	}

	/*lock the flash*/
	flash_lock();


	return status;
}


uint8_t flash_program(uint32_t mem_address, uint8_t payload_length, uint8_t *data)
{
	uint8_t status = FLASH_OK;

	/*unlock the flash*/
	flash_unlock();

	/*wait till busy is cleared*/
	flash_wait_busy();

	/*Clear previous error flags*/
   FLASH->SR |= FLASH_SR_PGSERR |
				 FLASH_SR_PGPERR |
				 FLASH_SR_PGAERR |
				 FLASH_SR_WRPERR |
				 FLASH_SR_OPERR;

	/*Clear the PSIZE bit in FLASH_CR (PSIZE = 8bit)*/
	FLASH->CR &=~ FLASH_CR_PSIZE;

	/*Set PG bit in FLASH_CR*/
	FLASH->CR |= FLASH_CR_PG;

	/*Perform write operation*/
	for(uint8_t i=0; i<payload_length; i++)
	{
		/*wait till busy is cleared*/
		flash_wait_busy();

		/*Write 1 byte*/
		*(volatile uint8_t *)mem_address = data[i];

		/*wait till busy is cleared*/
		flash_wait_busy();

		/*Check for errors*/
	    if(flash_program_error_check() != FLASH_OK)
	    {
	        status = FLASH_FAIL;
	        break;
	    }

		/*Move to next address*/
		mem_address++;
	}

	/*wait till busy is cleared*/
	flash_wait_busy();

	/*Clear the PG bit in FLASH_CR*/
	FLASH->CR &=~ FLASH_CR_PG;

	/*Check for any program error*/
	status = flash_program_error_check();

	/*lock the flash*/
	flash_lock();

	return status;

}

uint8_t flash_enable_rw_protection(uint8_t sector_mask, uint8_t protection_mode)
{
	/*
	 * mode 0: write protect
	 * mode 1: PCROP (read + write)
	 * */
	uint8_t status = FLASH_OK;


	/*unlock flash*/
	flash_unlock();

	/*Unlock the OPTCR register*/
	flash_option_unlock();

	/*wait until the busy flag is cleared*/
	flash_wait_busy();

	/*write the desired option in FLASH_OPTCR*/
	if(protection_mode == 1)
	{
		/*write protect*/

//		/*Set SPRMOD=0 in FLASH_OPTCR*/
//		FLASH->OPTCR &=~ FLASH_OPTCR_SPRMOD;

		/*clear WRP bit*/
		FLASH->OPTCR |= FLASH_OPTCR1_nWRP;

	     /*
	         * nWRP bits:
	         * 1 = Not protected
	         * 0 = Protected
	      */

		/*Protect requested sectors*/
		FLASH->OPTCR &=~ (sector_mask << FLASH_OPTCR1_nWRP_Pos);
	}
	else if (protection_mode == 2)
	{
		/*read + write protect*/

		/*Set the SPRMOD=1 in FLASH_OPTCR*/
		FLASH->OPTCR |= FLASH_OPTCR_SPRMOD;

		/*clear WRP bit*/
		FLASH->OPTCR &=~ FLASH_OPTCR1_nWRP;

	     /*
	         * nWRP bits:
	         * 0 = Not PCROP protected
	         * 1 = PCROP Protected
	      */

		/*Specify the sector for protection*/
		FLASH->OPTCR &=~ (sector_mask << FLASH_OPTCR1_nWRP_Pos);
	}
	else
	{
		return FLASH_FAIL;
	}

	/*set the OPTSTRT in FLASH_OPTCR*/
    FLASH->OPTCR |= FLASH_OPTCR_OPTSTRT;

	/*wait until the busy flag is cleared*/
	flash_wait_busy();

	/*check for error*/
	status = flash_error_check();

	/*Lock the OPTCR register*/
	flash_option_lock();

	/*lock flash*/
	flash_lock();

	return status;
}

uint8_t flash_disable_rw_protection(void)
{
	uint8_t status = FLASH_OK;

	/*Unlock flash*/
	flash_unlock();

	/*Unlock the OPTCR register*/
	flash_option_unlock();

	/*wait until the busy flag is cleared*/
	flash_wait_busy();

	/*Disable PCROP mode*/
	FLASH->OPTCR &=~ FLASH_OPTCR_SPRMOD;

	/*Disable write protection on all sector*/
	FLASH->OPTCR |= FLASH_OPTCR1_nWRP;

	/*set the OPTSTRT in FLASH_OPTCR*/
    FLASH->OPTCR |= FLASH_OPTCR_OPTSTRT;

	/*wait until the busy flag is cleared*/
	flash_wait_busy();

    /* Check for errors */
    status = flash_error_check();

	/*Lock the OPTCR register*/
	flash_option_lock();

	/*Lock flash*/
	flash_lock();

	return status;

}

uint8_t flash_get_sector_status(void)
{
	uint8_t status;

	/* Read the nWRP bits [23:16] */
	status = (FLASH->OPTCR >> FLASH_OPTCR1_nWRP_Pos) & 0xFF;

	return status;
}

static uint8_t flash_mass_erase(void)
{
	uint8_t status = FLASH_OK;

	/*unlock the flash*/
	flash_unlock();

	/*wait till busy is cleared*/
	flash_wait_busy();

    /* Clear previous error flags */
    FLASH->SR |= FLASH_SR_PGSERR |
                 FLASH_SR_PGPERR |
                 FLASH_SR_PGAERR |
                 FLASH_SR_WRPERR |
                 FLASH_SR_OPERR;

	/*Set the MER bit in FLASH_CR*/
	FLASH->CR |= FLASH_CR_MER;

	/*Set STRT bit in the FLASH_CR*/
	FLASH->CR |= FLASH_CR_STRT;

	/*wait till busy is cleared*/
	flash_wait_busy();

	/*Clear the MER bit in FLASH_CR*/
	FLASH->CR &=~ FLASH_CR_MER;

	/*Check for error in FLASH_SR*/
	status = flash_error_check();

	/*lock the flash*/
	flash_lock();

	return status;
}

static uint8_t flash_sector_erase(uint8_t sector)
{
	uint8_t status = FLASH_OK;

	/*wait till busy is cleared*/
	flash_wait_busy();

    /* Clear previous error flags */
    FLASH->SR |= FLASH_SR_PGSERR |
                 FLASH_SR_PGPERR |
                 FLASH_SR_PGAERR |
                 FLASH_SR_WRPERR |
                 FLASH_SR_OPERR;

	/*Set SER in FLASH_CR*/
	FLASH->CR |= FLASH_CR_SER;

	/*Select sector using SNB in FLASH_CR*/
	FLASH->CR |= (sector << FLASH_CR_SNB_Pos);

	/*Set the STRT bit in FLASH_CR*/
	FLASH->CR |= FLASH_CR_STRT;

	/*wait till busy is cleared*/
	flash_wait_busy();

	/*Clear SER bit in FLASH_CR*/
	FLASH->CR &= ~FLASH_CR_SER;

	/*Clear SNB bit in FLASH_CR*/
	FLASH->CR &= ~(0xF << FLASH_CR_SNB_Pos);

	/*Check for error in FLASH_SR*/
	status = flash_error_check();

	return status;
}

static uint8_t flash_program_error_check(void)
{
	uint8_t status = FLASH_OK;

	if(FLASH->SR & (FLASH_SR_PGSERR |
					FLASH_SR_PGPERR |
					FLASH_SR_PGAERR |
					FLASH_SR_WRPERR))
	{
		status = FLASH_FAIL;
	}

	return status;
}

static uint8_t flash_error_check(void)
{
	uint8_t status = FLASH_OK;

	if(FLASH->SR & (FLASH_SR_PGSERR |
					FLASH_SR_PGPERR |
					FLASH_SR_PGAERR |
					FLASH_SR_WRPERR |
					FLASH_SR_OPERR))
	{
		status = FLASH_FAIL;
	}

	return status;
}

void flash_lock(void)
{
	/*Lock the flash control register*/
	FLASH->CR |= FLASH_CR_LOCK;
}

void flash_unlock(void)
{
	if (FLASH->CR & FLASH_CR_LOCK)
	{
		/*Unlock the flash by using the keys*/
		FLASH->KEYR = FKEY1;
		FLASH->KEYR = FKEY2;
	}
}

static void flash_wait_busy(void)
{
	/*Wait until flash busy flag is cleared*/
	while(FLASH->SR & FLASH_SR_BSY){}
}

static void flash_option_lock(void)
{
	/*Locks the flash option control register*/
	FLASH->OPTCR |= FLASH_OPTCR_OPTLOCK;
}

static void flash_option_unlock(void)
{
	if(FLASH->OPTCR & FLASH_OPTCR_OPTLOCK)
	{
		/*Unlocks the flash option control register*/
		FLASH->OPTKEYR |= OPTKEY1;
		FLASH->OPTKEYR |= OPTKEY2;
	}
}
