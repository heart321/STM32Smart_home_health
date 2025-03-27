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
#include "usart.h"



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
/**
 * @brief  	关闭pm2.5串口接收函数
 * @param   None
 * @retval  None
 */
void pm25_rev_data_stop(void)
{
	HAL_UART_Abort(&huart6);
}



