/*
 * crc.h
 *
 *  Created on: 30 Jul 2026
 *      Author: Senthil
 */

#ifndef CRC_H_
#define CRC_H_

#include "stm32f411xe.h"
#include <stdint.h>
#include "error.h"

error_t crc_verify(uint8_t *data, uint8_t length, uint32_t received_crc);


#endif /* CRC_H_ */
