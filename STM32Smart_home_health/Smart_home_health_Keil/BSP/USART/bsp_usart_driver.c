/*
 * @file      bsp_usart_driver.c
 *
 * @brief     初始化串口
 *
 * @version   V1.0
 *
 * @date     2025/1/16 星期四
 *
 * @auther   chenxuan_123
 */


#include "bsp_usart_driver.h"
#include "FreeRTOS.h"
#include "task.h"
#include "usart.h"

/*
 * @brief  串口2  发送数据
 */
void wifi_usart2_Send(uint8_t *data,uint16_t len)
{
    HAL_UART_Transmit(&huart2,data,len,1000);
}

/*
 * @brief  串口2 接收数据
 */
void wifi_usart2_Receive(uint8_t *data,uint16_t len)
{
    HAL_UART_Receive_IT(&huart2,data,len);
	
}

/*
 * @brief  串口1 发送数据
 * @param   None
 * @retval  None
 */
void Debug_usart1_Send(uint8_t *data,uint16_t len)
{
    HAL_UART_Transmit(&huart1,data,len,portMAX_DELAY);
}

/*
* @brief  将串口1重定向为printf 用于DEBUG 
 */
int fputc(int c, FILE* file)
{
    HAL_UART_Transmit(&huart1, (uint8_t*)&c, 1,1000);
    return c;
}
