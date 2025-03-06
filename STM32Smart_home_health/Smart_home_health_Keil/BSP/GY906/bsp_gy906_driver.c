#include "bsp_gy906_driver.h"
#include "bsp_usart_driver.h"

#define MLX90614_ADDR		 0x5A  // 设备地址
#define MLX90614_TA          0x06  // 环境温度
#define MLX90614_TOBJ1       0x07  // 物体温度


extern I2C_HandleTypeDef hi2c1;


/**
  * @brief  读取温度数据
  * @param  None
  * @retval 温度值（摄氏度）
  */
float mlx90614_readTemp(uint8_t reg)
{
	uint8_t data[3] = {0};
	uint16_t temp;
	float result;
	
	
	// 使用I2C读取寄存器数据
	if(HAL_I2C_Mem_Read(&hi2c1,MLX90614_ADDR << 1,reg,I2C_MEMADD_SIZE_8BIT,data,3,100) != HAL_OK)
	{
		// 错误处理
		DEBUG_LOG("MLX90614 I2C 读取失败，错误码: %d\n", HAL_I2C_GetError(&hi2c1));
		return -1.0f; // 返回错误值，而不是死循环
	}
	
	// 转换温度数据
	temp = (data[1] << 8) | data[0];
	result = (float)temp * 0.02 - 273.15;
	return result;


}

/**
  * @brief  读取物体温度数据
  * @param  None
  * @retval 温度值（摄氏度）
  */
float readObjectTemp(void)
{
	return mlx90614_readTemp(MLX90614_TOBJ1);

}

/**
  * @brief  读取环境温度数据
  * @param  None
  * @retval 温度值（摄氏度）
  */
float readAmbientTemp(void)
{
	return mlx90614_readTemp(MLX90614_TA);
}


