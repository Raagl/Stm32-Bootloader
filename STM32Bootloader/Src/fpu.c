
#include "fpu.h"

void fpu_enable()
{
	// ENABLE FPU: ENABLE CP10 AND CP11 FULL ACCESS
	SCB->CPACR |= (1U<<20);
	SCB->CPACR |= (1U<<21);
	SCB->CPACR |= (1U<<22);
	SCB->CPACR |= (1U<<23);
}
