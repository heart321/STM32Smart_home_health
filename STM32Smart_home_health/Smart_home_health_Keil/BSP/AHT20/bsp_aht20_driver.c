/*
 * @file      bsp_aht20_driver.c
 *
 * @brief     AHT20温湿度传感器驱动
 *
 * @version   V1.0
 *
 * @date     2025/1/27 星期一
 *
 * @auther   chenxuan_123
 */

#include "bsp_aht20_driver.h"


#define AHT20_ADDRESS 0x70

/*iic 1句柄*/
I2C_HandleTypeDef hi2c1;

/*
 * @brief   AHT20初始化
 * @param   None
 * @retval  None
 */
void aht20_init(void)
{

	// 1.开启时钟
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_I2C1_CLK_ENABLE();
	
	
	// 2.配置GPIO
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Pin = GPIO_PIN_6 | GPIO_PIN_7;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.Alternate = GPIO_AF4_I2C1;
	HAL_GPIO_Init(GPIOB,&GPIO_InitStruct);
	
	
	// 3.配置iic
	hi2c1.Instance = I2C1;
	hi2c1.Init.ClockSpeed = 100000;
	hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
	hi2c1.Init.OwnAddress1 = 0;
	hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
	hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
	hi2c1.Init.OwnAddress2 = 0;
	hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
	hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
	if(HAL_I2C_Init(&hi2c1) != HAL_OK)
	{
		while(1);
	
	}
	
	// 4.初始化AHT20
	uint8_t readBuffer;
	HAL_Delay(40);
	HAL_I2C_Master_Receive(&hi2c1,AHT20_ADDRESS,&readBuffer,1,HAL_MAX_DELAY);
	if((readBuffer & 0x08) == 0x00)
	{
		uint8_t sendBuffer[3] = {0xBE , 0x08 , 0x00};
		HAL_I2C_Master_Transmit(&hi2c1,AHT20_ADDRESS,sendBuffer,3,HAL_MAX_DELAY);
	}
}


/*
 * @brief   读取AHT20温湿度数据
 * @param   temp 温度数据 humi 湿度数据
 * @retval  None
 */

void aht20_read(float* temp,float* humi)
{
	uint8_t sendBuffer[3] = {0xAC, 0x33, 0x00};
	uint8_t readBuffer[6];
	
	HAL_I2C_Master_Transmit(&hi2c1,AHT20_ADDRESS,sendBuffer,3,HAL_MAX_DELAY);
	HAL_Delay(75);
	
	HAL_I2C_Master_Receive(&hi2c1,AHT20_ADDRESS,readBuffer,6,HAL_MAX_DELAY);
	
	if((readBuffer[0] & 0x80) == 0x00)
	{
		uint32_t humi_data = ((uint32_t)readBuffer[1] << 12) | 
                     ((uint32_t)readBuffer[2] << 4) | 
                     ((readBuffer[3] >> 4) & 0x0F);
		*humi = (humi_data * 100.0f) / (1 << 20);

		uint32_t temp_data = ((uint32_t)(readBuffer[3] & 0x0F) << 16) | 
                     ((uint32_t)readBuffer[4] << 8) | 
                     readBuffer[5];
		*temp = (temp_data * 200.0f) / (1 << 20) - 50;	
	
	
	}
}



