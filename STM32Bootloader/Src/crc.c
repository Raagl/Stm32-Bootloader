/*
 * crc.c
 *
 *  Created on: 30 Jul 2026
 *      Author: Senthil
 */

#include "crc.h"

error_t crc_verify(uint8_t *data, uint8_t length, uint32_t received_crc)
{
	error_t status = ERR_OK;
	uint32_t calculated_crc = 0;

	/*Enable Clock to CRC*/
	RCC->AHB1ENR |= RCC_AHB1ENR_CRCEN;

	/*Reset CRC*/
	CRC->CR |= 	CRC_CR_RESET;

	/*Feed the data*/
    for (uint32_t i = 0; i < length; i++)
    {
        CRC->DR = (uint32_t)data[i];
    }

	/*Store the Calculated CRC*/
	calculated_crc = CRC->DR;

	/*Reset CRC*/
	CRC->CR |= 	CRC_CR_RESET;

	/*Compare them*/
	if (!(calculated_crc == received_crc))
	{
		status = ERR_FAIL;
	}

	return status;
}
