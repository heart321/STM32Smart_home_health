/*
 * @file      bsp_airquality_driver.c
 *
 * @brief     空气质量传感器 测量 CO2 TVO HCHO
 *
 * @version   V1.0
 *
 * @date     2025/2/6 星期四
 *
 * @auther   chenxuan_123
 */

#include "bsp_airquality_driver.h"
#include "bsp_usart_driver.h"

UART_HandleTypeDef huart4;



/**
  * @brief  空气质量数据
  * @retval None
  */
void Airquality_usart4_init(void)
{
    // 1.开启UART4时钟 GPIOA时钟 
	if(__HAL_RCC_GPIOA_IS_CLK_DISABLED())
	{
		__HAL_RCC_GPIOA_CLK_ENABLE();
	}

	__HAL_RCC_UART4_CLK_ENABLE();
	
	// 2.配置GPIO口
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.Alternate = GPIO_AF8_UART4;
	HAL_GPIO_Init(GPIOA,&GPIO_InitStruct);
	
	
	// 3.配置串口参数
	huart4.Instance = UART4;
	huart4.Init.BaudRate = 9600;
	huart4.Init.WordLength = UART_WORDLENGTH_8B;
	huart4.Init.StopBits = UART_STOPBITS_1;
	huart4.Init.Parity = UART_PARITY_NONE;               				/*不启用奇偶校验*/
	huart4.Init.Mode = UART_MODE_TX_RX;									/*设置 UART4 的模式为收发（TX 和 RX）*/
	huart4.Init.HwFlowCtl = UART_HWCONTROL_NONE;						/*不启用硬件流控*/
	huart4.Init.OverSampling = UART_OVERSAMPLING_16;					/*设置过采样比为16*/
    if (HAL_UART_Init(&huart4) != HAL_OK)
    {
        DEBUG_LOG("UART4 初始化失败\n");
        while (1);
    }
	

	//开启串口4中断
	HAL_NVIC_SetPriority(UART4_IRQn,6,0);
	HAL_NVIC_EnableIRQ(UART4_IRQn);
	
}

/**
 * @brief  	串口4 发送函数
 * @param   data 要发送的数据 
 * 			len 数据长度
 * @retval  None
 */
void Airquality_send_data(uint8_t* data,uint16_t len)
{
	/*普通发送*/
	HAL_UART_Transmit(&huart4, data, len,HAL_MAX_DELAY);
	/*中断发送*/
	//HAL_UART_Transmit_IT(&huart4, data, len);
	/*配合DMA接收*/
	//HAL_UART_Transmit_IT(&huart4, data, len);
}

/**
 * @brief  	串口4 接收函数
 * @param   data 存放接收的数据 
 * 			len 数据长度
 * @retval  None
 */
void Airquality_rev_data(uint8_t* data,uint16_t len)
{
	/*普通接收*/
	//HAL_UART_Receive(&huart4,data,len,HAL_MAX_DELAY);
	/*中断发送 只能接收定长数据*/  
	//HAL_UART_Receive_IT(&huart4,data,len);
	/*配合DMA接收*/
	//HAL_UART_Receive_DMA(&huart4, data, len);
	// 接收不定长字节
	if (HAL_UARTEx_ReceiveToIdle_IT(&huart4, data, len) != HAL_OK) {
        DEBUG_LOG("UART4 接收启动失败 错误码%d\n",huart4.ErrorCode);
    } 
}

