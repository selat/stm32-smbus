#include "utils.h"

#include <system_stm32f4xx.h>
#include <stm32f4xx_tim.h>
#include <stm32f4xx_rcc.h>

static volatile uint32_t timing_delay = 0;

void DelayMs(uint32_t delay)
{
	uint32_t s = TIM_GetCounter(TIM2);
	while((TIM_GetCounter(TIM2) - s) < delay);
}

void DelayUs(uint32_t delay)
{
	uint32_t s = TIM_GetCounter(TIM5);
	while((TIM_GetCounter(TIM5) - s) < delay);
}

uint32_t GetTimeMs()
{
	return TIM_GetCounter(TIM2);
}

uint32_t GetTimeUs()
{
	return TIM_GetCounter(TIM5);
}

void TIM_Init()
{
	TIM_TimeBaseInitTypeDef tim_conf;
	RCC_ClocksTypeDef clocks;
	uint32_t base_frequency;
	/* Setup TIM2 for 1000 Hz and TIM5 for 1000_000 Hz */
	RCC_GetClocksFreq(&clocks);
	base_frequency = clocks.PCLK2_Frequency;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);

	tim_conf.TIM_ClockDivision = TIM_CKD_DIV1;
	tim_conf.TIM_CounterMode = TIM_CounterMode_Up;
	tim_conf.TIM_Period = 0xFFFFFFFF;

	/* TODO: understand why we need to divide base_frequency by 500 and not by 1000 */
	tim_conf.TIM_Prescaler = base_frequency / 500 - 1;
	TIM_TimeBaseInit(TIM2, &tim_conf);
	tim_conf.TIM_Prescaler = base_frequency / 500000 - 1;
	TIM_TimeBaseInit(TIM5, &tim_conf);

	TIM_Cmd(TIM2, ENABLE);
	TIM_Cmd(TIM5, ENABLE);
}

void GPIO_PinSetOutput(GPIO_TypeDef* GPIOx, uint16_t pin_id)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = pin_id;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOx, &GPIO_InitStructure);
}

void GPIO_PinSetInput(GPIO_TypeDef* GPIOx, uint16_t pin_id)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = pin_id;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOx, &GPIO_InitStructure);
}
