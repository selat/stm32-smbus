#include "mlx90614.h"

#include <stdio.h>

#include <stm32f4xx_gpio.h>

#include "smbus.h"
#include "utils.h"

static uint16_t to_max, to_min;
static uint8_t mlx90614_address = 0x00;

/**
 * @brief  Enable SMBus communication mode for MLX90614.
 * To select SMBus mode instead of PWM for MLX90614 it's just required
 * to reset SCL line for more than 2ms.
 * @param  None
 * @retval None
 */
void MLX90614_DisablePWM(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = SMBus_SCL_Pin;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(SMBus_Port, &GPIO_InitStructure);

	GPIO_WriteBit(GPIOB, SMBus_SCL_Pin, Bit_RESET);
	DelayMs(10);
	GPIO_WriteBit(GPIOB, SMBus_SCL_Pin, Bit_SET);
}

/**
 * @brief  Initialize MLX90614
 * @param  None
 * @retval None
 */
void MLX90614_Init(void)
{
	SMBus_ReadWord(mlx90614_address, &to_max, 0x20);
	SMBus_ReadWord(mlx90614_address, &to_min, 0x21);
}

/**
 * @brief  Test connection with MLX90614
 * @param  None
 * @retval 1 - sensor works properly, 0 - error
 */
int MLX90614_TestConnection(void)
{
	uint16_t tmp;
	SMBus_ReadWord(mlx90614_address, &tmp, 0x06);
	return 1;
}

uint16_t MLX90614_ReadAmbientTemp(void)
{
	uint16_t tmp;
	SMBus_ReadWord(mlx90614_address, &tmp, 0x06);
	uint16_t res = tmp;
	return res;
}

uint16_t MLX90614_ReadObjectTemp(void)
{
	uint16_t tmp;
	SMBus_ReadWord(mlx90614_address, &tmp, 0x07);
	uint16_t res = tmp;

	MLX90614_SetAddress(0x02);
	SMBus_ReadWord(mlx90614_address, &tmp, 0x2E);
	printf("Address: %"PRIu16"\r\n", tmp);
	return res;
}

void MLX90614_SetAddress(uint8_t address)
{
	SMBus_WriteWord(mlx90614_address, address, 0x2E);
}
