#include <stm32f4xx.h>

#include <stm32f4xx_rcc.h>
#include <stm32f4xx_gpio.h>
#include <stm32f4xx_misc.h>
#include <stm32f4xx_exti.h>
#include <stm32f4xx_syscfg.h>

#include <stdio.h>
#include <stdlib.h>

#include "usart.h"
#include "rcc.h"
#include "utils.h"

#include <math.h>

int main(void) {
	uint32_t address = 0;
	SystemInit();
	__enable_irq();

	while(1) {
	}
	return 0;
}
