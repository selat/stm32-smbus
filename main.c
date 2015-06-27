#include <stm32f4xx.h>

#include <stdio.h>

#include "usart.h"
#include "rcc.h"
#include "utils.h"
#include "mlx90614.h"
#include "smbus.h"

int main(void) {
	SystemInit();
	__enable_irq();
	RCC_init();
	TIM_Init();
	USART_init();

	SMBus_init();
	MLX90614_Init();
	MLX90614_TestConnection();

	printf("ok!\r\n");

	while(1) {
//		float temp = MLX90614_ReadTemperature() * 0.02f - 38.2f;
		uint16_t temp_ambient, temp_object;
		temp_ambient = MLX90614_ReadAmbientTemp();
		temp_object = MLX90614_ReadObjectTemp();
		printf("MLX90614: %"PRIu16", %"PRIu16"\r\n", temp_ambient, temp_object);
		DelayMs(100);
	}
	return 0;
}
