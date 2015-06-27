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
		uint16_t temp_ambient, temp_object;
		temp_ambient = MLX90614_ReadAmbientTemp();
		temp_object = MLX90614_ReadObjectTemp();
		float temp1 = (temp_ambient - 0x2DE4) * 0.02f - 38.2f;
		float temp2 = (temp_object) * 0.02f - 273.15f;
		printf("MLX90614: %d.%d %d.%d\r\n", (int)temp1, (int)(temp1 * 100) % 100, (int)temp2, (int)(temp2 * 100) % 100);
		DelayMs(100);
	}
	return 0;
}
