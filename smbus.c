/*
  ============================================================================================
  HMC5883L device SMBus library code for ARM STM32F103xx is placed under the MIT license
  Copyright (c) 2012 Harinadha Reddy Chintalapalli

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
  THE SOFTWARE.
  ================================================================================================
*/

#include <stm32f4xx_i2c.h>

#include "smbus.h"

void SMBus_init()
{
    I2C_InitTypeDef SMBus_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;

    /* Enable I2C and GPIO clocks */
    RCC_APB1PeriphClockCmd(SMBus_RCC_Periph, ENABLE);
    RCC_AHB1PeriphClockCmd(SMBus_RCC_Port, ENABLE);

    /* Configure SMBus pins: SCL and SDA */
    GPIO_InitStructure.GPIO_Pin = SMBus_SCL_Pin | SMBus_SDA_Pin;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(SMBus_Port, &GPIO_InitStructure);

    GPIO_PinAFConfig(SMBus_Port, SMBus_SCL_PinSource, SMBus_GPIO_AF);
    GPIO_PinAFConfig(SMBus_Port, SMBus_SDA_PinSource, SMBus_GPIO_AF);

    /* SMBus configuration */
    SMBus_InitStructure.I2C_Mode = I2C_Mode_SMBusHost;
    SMBus_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
    SMBus_InitStructure.I2C_OwnAddress1 = 0x01;
    SMBus_InitStructure.I2C_Ack = I2C_Ack_Enable;
    SMBus_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
    SMBus_InitStructure.I2C_ClockSpeed = SMBus_Speed;

    /* Apply SMBus configuration after enabling it */
    I2C_Init(SMBus_NAME, &SMBus_InitStructure);

    I2C_Cmd(SMBus_NAME, ENABLE);
}


/**
 * @brief  Writes one byte to the peripheral device.
 * @param  slaveAddr : slave address.
 * @param  pBuffer : pointer to the buffer  containing the data to be written to the peripheral device.
 * @param  WriteAddr : address of the register in which the data will be written.
 * @retval None
 */
void SMBus_WriteWord(uint8_t slaveAddr, uint16_t data, uint8_t WriteAddr)
{
	volatile uint32_t pec = SMBus_CRC8(((uint32_t)WriteAddr << 16) | ((uint32_t)(data & 0xFF) << 8) | (data >> 8));
    I2C_GenerateSTART(SMBus_NAME, ENABLE);
    while (!I2C_CheckEvent(SMBus_NAME, I2C_EVENT_MASTER_MODE_SELECT));

    I2C_Send7bitAddress(SMBus_NAME, slaveAddr, I2C_Direction_Transmitter);
    while (!I2C_CheckEvent(SMBus_NAME, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

    I2C_SendData(SMBus_NAME, WriteAddr);
    while (!I2C_CheckEvent(SMBus_NAME, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

    I2C_SendData(SMBus_NAME, data & 0xFF);
    while (!I2C_CheckEvent(SMBus_NAME, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
    I2C_SendData(SMBus_NAME, (data >> 8));
    while (!I2C_CheckEvent(SMBus_NAME, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
    I2C_SendData(SMBus_NAME, pec);
    while (!I2C_CheckEvent(SMBus_NAME, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

    I2C_GenerateSTOP(SMBus_NAME, ENABLE);
}

uint8_t SMBus_ReadByte(uint8_t slaveAddr, uint8_t readAddr)
{
	uint16_t res;
	SMBus_ReadWord(slaveAddr, &res, readAddr);
	return res;
}

/**
 * @brief  Reads a block of data from the peripheral device.
 * @param  slaveAddr  : slave address.
 * @param  pBuffer : pointer to the buffer that receives the data read from the peripheral device.
 * @param  ReadAddr : Peripheral device's internal address to read from.
 * @param  NumByteToRead : number of bytes to read from the peripheral device.
 * @retval None
 */
void SMBus_ReadWord(uint8_t slaveAddr, uint16_t* data, uint8_t ReadAddr)
{
    // ENTR_CRT_SECTION();
	uint8_t buff[2];
	volatile uint8_t pec, expected_pec;

    while (I2C_GetFlagStatus(SMBus_NAME, I2C_FLAG_BUSY));

    I2C_GenerateSTART(SMBus_NAME, ENABLE);

    while (!I2C_CheckEvent(SMBus_NAME, I2C_EVENT_MASTER_MODE_SELECT));
    I2C_Send7bitAddress(SMBus_NAME, slaveAddr, I2C_Direction_Transmitter);
    while (!I2C_CheckEvent(SMBus_NAME, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

    I2C_Cmd(SMBus_NAME, ENABLE);

    I2C_SendData(SMBus_NAME, ReadAddr);
    while (!I2C_CheckEvent(SMBus_NAME, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

    I2C_GenerateSTART(SMBus_NAME, ENABLE);
    while (!I2C_CheckEvent(SMBus_NAME, I2C_EVENT_MASTER_MODE_SELECT));

    I2C_Send7bitAddress(SMBus_NAME, slaveAddr, I2C_Direction_Receiver);
    while (!I2C_CheckEvent(SMBus_NAME, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));

    while (!I2C_CheckEvent(SMBus_NAME, I2C_EVENT_MASTER_BYTE_RECEIVED));
    buff[0] = I2C_ReceiveData(SMBus_NAME);
    while (!I2C_CheckEvent(SMBus_NAME, I2C_EVENT_MASTER_BYTE_RECEIVED));
    buff[1] = I2C_ReceiveData(SMBus_NAME);
    while (!I2C_CheckEvent(SMBus_NAME, I2C_EVENT_MASTER_BYTE_RECEIVED));
    pec = I2C_ReceiveData(SMBus_NAME);
    expected_pec = SMBus_CRC8(((uint32_t)ReadAddr << 16) | ((uint32_t)buff[0] << 8) | buff[1]);
    I2C_GenerateSTOP(SMBus_NAME, ENABLE);
    *data = ((uint16_t)buff[1] << 8) | buff[0];
    // EXT_CRT_SECTION();
}

/**
 * Write multiple bits in an 8-bit device register.
 * @param slaveAddr SMBus slave device address
 * @param regAddr Register regAddr to write to
 * @param bitStart First bit position to write (0-7)
 * @param length Number of bits to write (not more than 8)
 * @param data Right-aligned value to write
 */
void SMBus_WriteBits(uint8_t slaveAddr, uint8_t regAddr, uint8_t bitStart, uint8_t length, uint8_t data)
{
    uint8_t tmp = SMBus_ReadByte(slaveAddr, regAddr);
    uint8_t mask = ((1 << length) - 1) << (bitStart - length + 1);
    data <<= (bitStart - length + 1); // shift data into correct position
    data &= mask; // zero all non-important bits in data
    tmp &= ~(mask); // zero all important bits in existing byte
    tmp |= data; // combine data with existing byte
    SMBus_WriteWord(slaveAddr, tmp, regAddr);
}

/**
 * Write a single bit in an 8-bit device register.
 * @param slaveAddr SMBus slave device address
 * @param regAddr Register regAddr to write to
 * @param bitNum Bit position to write (0-7)
 * @param value New bit value to write
 */
void SMBus_WriteBit(uint8_t slaveAddr, uint8_t regAddr, uint8_t bitNum, uint8_t data)
{
    uint8_t tmp = SMBus_ReadByte(slaveAddr, regAddr);
    tmp = (data != 0) ? (tmp | (1 << bitNum)) : (tmp & ~(1 << bitNum));
    SMBus_WriteWord(slaveAddr, tmp, regAddr);
}

/**
 * Read multiple bits from an 8-bit device register.
 * @param slaveAddr SMBus slave device address
 * @param regAddr Register regAddr to read from
 * @param bitStart First bit position to read (0-7)
 * @param length Number of bits to read (not more than 8)
 * @param data Container for right-aligned value (i.e. '101' read from any bitStart position will equal 0x05)
 * @param timeout Optional read timeout in milliseconds (0 to disable, leave off to use default class value in readTimeout)
 */
void SMBus_ReadBits(uint8_t slaveAddr, uint8_t regAddr, uint8_t bitStart, uint8_t length, uint8_t *data)
{
    uint8_t tmp = SMBus_ReadByte(slaveAddr, regAddr);
    uint8_t mask = ((1 << length) - 1) << (bitStart - length + 1);
    tmp &= mask;
    tmp >>= (bitStart - length + 1);
    *data = tmp;
}

/**
 * Read a single bit from an 8-bit device register.
 * @param slaveAddr SMBus slave device address
 * @param regAddr Register regAddr to read from
 * @param bitNum Bit position to read (0-7)
 * @param data Container for single bit value
 * @param timeout Optional read timeout in milliseconds (0 to disable, leave off to use default class value in readTimeout)
 */
void SMBus_ReadBit(uint8_t slaveAddr, uint8_t regAddr, uint8_t bitNum, uint8_t *data)
{
    uint8_t tmp = SMBus_ReadByte(slaveAddr, regAddr);
    *data = tmp & (1 << bitNum);
}

/**
 * Calculate CRC8 sum for a word
 * @param data input word
 * @return calculated CRC8 for an input word
 */
uint8_t SMBus_CRC8(uint32_t data)
{
	uint32_t msg = data << 8;
	uint32_t key = 0x107 << 23;
	uint32_t mask = 1 << 31;
	while(mask > 0x80) {
		if(mask & msg) {
			msg ^= key;
		}
		key >>= 1;
		mask >>= 1;
	}
	return msg;
}
