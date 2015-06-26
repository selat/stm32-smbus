#include "rcc.h"

#include <stm32f4xx_rcc.h>

void RCC_init()
{
	// Drop all previous settings of rcc
	RCC_DeInit();
	RCC_PLLCmd(DISABLE);
	RCC_PLLConfig(RCC_PLLSource_HSI, 25, 300, 2, 7);
	RCC_PLLCmd(ENABLE);
	while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET);
	RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
	while(RCC_GetSYSCLKSource() != 0x08);
	RCC_HCLKConfig(RCC_SYSCLK_Div1);
	RCC_PCLK2Config(RCC_HCLK_Div4);
	RCC_PCLK1Config(RCC_HCLK_Div4);
}
