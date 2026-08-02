/*
 * bsp.h
 *
 *  Created on: 26 Jul 2026
 *      Author: Senthil
 */

#ifndef BSP_H_
#define BSP_H_

#include <stdbool.h>
#include "stm32f411xe.h"

void led_init(void);
void led_on(void);
void led_off(void);
void button_init(void);
bool get_button_state(void);


#endif /* BSP_H_ */
