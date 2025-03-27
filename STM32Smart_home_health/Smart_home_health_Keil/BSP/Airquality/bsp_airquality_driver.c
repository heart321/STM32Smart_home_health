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
#include "usart.h"


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

/**
 * @brief  	关闭pm2.5串口接收函数
 * @param   None
 * @retval  None
 */
void Airquality_rev_data_stop(void)
{
	HAL_UART_Abort(&huart4);
}
