/*
 * @file      bsp_pm2.5_driver.c
 *
 * @brief     测量空气中的PM2.5浓度
 *
 * @version   V1.0
 *
 * @date     2025/2/6 星期四
 *
 * @auther   chenxuan_123
 *
 * @tips     1tab = 4spaces
 */
#include "bsp_pm2.5_driver.h"

UART_HandleTypeDef huart6;

/**
 * @brief  	pm2.5串口初始化
 * @param   None
 * @retval  None
 */
void pm25_usart6_init(void)
{
	// 1.初始时钟 GPIOC USART6
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_USART6_CLK_ENABLE();
	
	// 2.初始化GPIO
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Pin = GPIO_PIN_6 | GPIO_PIN_7;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.Alternate = GPIO_AF8_USART6;
	HAL_GPIO_Init(GPIOC,&GPIO_InitStruct);
	
	
	// 3.配置串口6
	huart6.Instance = USART6;
	huart6.Init.BaudRate = 9600;
	huart6.Init.WordLength = UART_WORDLENGTH_8B;
	huart6.Init.StopBits = UART_STOPBITS_1;
	huart6.Init.Parity = UART_PARITY_NONE;               				/*不启用奇偶校验*/
	huart6.Init.Mode = UART_MODE_TX_RX;									/*设置 USART6的模式为收发（TX 和 RX）*/
	huart6.Init.HwFlowCtl = UART_HWCONTROL_NONE;						/*不启用硬件流控*/
	huart6.Init.OverSampling = UART_OVERSAMPLING_16;					/*设置过采样比为16*/
	HAL_UART_Init(&huart6);
	
	
	// 4.打开串口6中断
	HAL_NVIC_SetPriority(USART6_IRQn,10,0);
	HAL_NVIC_EnableIRQ(USART6_IRQn);

}

/**
 * @brief  	pm2.5串口接收函数
 * @param   None
 * @retval  None
 */
void pm25_rev_data(uint8_t* data,uint16_t len)
{
	//HAL_UART_Receive(&huart6,data,len,100);
	HAL_UARTEx_ReceiveToIdle_IT(&huart6,data,len);
}




