/*
 * timebase.h
 *
 *  Created on: 26 Jul 2026
 *      Author: Senthil
 */

#ifndef TIMEBASE_H_
#define TIMEBASE_H_

#include <stdint.h>
#include "stm32f411xe.h"

void delay(uint32_t delay);
void timebase_init(void);
uint32_t get_tick(void);



#endif /* TIMEBASE_H_ */
