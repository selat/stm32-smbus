#include <stm32f4xx.h>

#include <stdio.h>

#include "usart.h"
#include "rcc.h"
#include "utils.h"

int main(void) {
	SystemInit();
	__enable_irq();
	RCC_init();
	TIM_Init();
	USART_init();

	printf("ok!\r\n");

	while(1) {
	}
	return 0;
}
