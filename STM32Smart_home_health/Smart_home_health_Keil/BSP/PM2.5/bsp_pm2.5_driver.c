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
#include "bsp_usart_driver.h"

UART_HandleTypeDef huart6;


/**
 * @brief  	pm2.5串口初始化
 * @param   None
 * @retval  None
 */
void pm25_usart6_init(void)
{
	
	if (__HAL_RCC_GPIOC_IS_CLK_DISABLED()) { __HAL_RCC_GPIOC_CLK_ENABLE(); }
    __HAL_RCC_USART6_CLK_ENABLE();

    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = GPIO_PIN_6 | GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF8_USART6;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    huart6.Instance = USART6;
    huart6.Init.BaudRate = 9600;
    huart6.Init.WordLength = UART_WORDLENGTH_8B;
    huart6.Init.StopBits = UART_STOPBITS_1;
    huart6.Init.Parity = UART_PARITY_NONE;
    huart6.Init.Mode = UART_MODE_TX_RX;
    huart6.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart6.Init.OverSampling = UART_OVERSAMPLING_16;
    if (HAL_UART_Init(&huart6) != HAL_OK)
    {
        DEBUG_LOG("USART6 初始化失败\n");
        while (1);
    }

    HAL_NVIC_SetPriority(USART6_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(USART6_IRQn);
}

/**
 * @brief  	pm2.5串口接收函数
 * @param   None
 * @retval  None
 */
void pm25_rev_data(uint8_t* data, uint16_t len)
{
    if (HAL_UARTEx_ReceiveToIdle_IT(&huart6, data, len) != HAL_OK)
    {
        DEBUG_LOG("USART6 接收启动失败, 错误码: %d\n", huart6.ErrorCode);
    }
}





