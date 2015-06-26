#include <stm32f4xx_rcc.h>
#include <stm32f4xx_gpio.h>
#include <stm32f4xx_usart.h>
#include <stm32f4xx_i2c.h>

#include <stdio.h>

#include "USART.h"

void USART_init()
{
	RCC_APB1PeriphClockCmd(USART_PERIPH_BUS_ID, ENABLE);
	RCC_AHB1PeriphClockCmd(USART_PERIPH_PIN_GROUP_BUS_ID, ENABLE);

	GPIO_InitTypeDef gpioConfig;
	gpioConfig.GPIO_Pin = USART_GPIO_PIN1 | USART_GPIO_PIN2;
	gpioConfig.GPIO_Mode = GPIO_Mode_AF;
	gpioConfig.GPIO_OType = GPIO_OType_PP;
	gpioConfig.GPIO_PuPd = GPIO_PuPd_UP;
	gpioConfig.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(USART_PIN_GROUP, &gpioConfig);

	GPIO_PinAFConfig(USART_PIN_GROUP, USART_GPIO_PIN_SOURCE1, USART_GPIO_AF);
	GPIO_PinAFConfig(USART_PIN_GROUP, USART_GPIO_PIN_SOURCE2, USART_GPIO_AF);

	USART_InitTypeDef usartConfig;
	usartConfig.USART_BaudRate = 38400;
	usartConfig.USART_WordLength = USART_WordLength_8b;
	usartConfig.USART_StopBits = USART_StopBits_1;
	usartConfig.USART_Parity = USART_Parity_No;
	usartConfig.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	usartConfig.USART_HardwareFlowControl =
		USART_HardwareFlowControl_None;
	USART_Init(USART_NAME, &usartConfig);

	USART_Cmd(USART_NAME, ENABLE);
}

unsigned char USART_ReadByteSync(USART_TypeDef *USARTx)
{
	while (USART_GetFlagStatus(USARTx, USART_SR_RXNE) == RESET);

	return (unsigned char)USART_ReceiveData(USARTx);
}

/* These functions redirect stdio to USART */
int _write(int file, char *s, int len)
{
	int i;
	for(i = 0; i < len; ++i) {
		USART_SendData(USART_NAME, s[i]);
		while(USART_GetFlagStatus(USART_NAME, USART_FLAG_TXE) == RESET);
	}
	return len;
}

int _read(int file, char *s, int len)
{
	*s = USART_ReadByteSync(USART_NAME);
	return 1;
}

int _isatty (int fd)
{
	return 1;
}
