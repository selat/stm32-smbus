#ifndef _UTILS_H_
#define _UTILS_H_

#include <inttypes.h>

#include <stm32f4xx_gpio.h>

void DelayMs(uint32_t delay);
void DelayUs(uint32_t delay);
uint32_t GetTimeMs();
uint32_t GetTimeUs();
void TIM_Init();

void GPIO_PinSetOutput(GPIO_TypeDef* GPIOx, uint16_t pin);
void GPIO_PinSetInput(GPIO_TypeDef* GPIOx, uint16_t pin);

#endif
