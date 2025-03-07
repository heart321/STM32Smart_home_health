/*
 * @file      app_air_task.c
 *
 * @brief     测量环境TVCO CO2 HCHO PM2.5
 *
 * @version   V1.0
 *
 * @date     2025/2/6 星期四
 *
 * @auther   chenxuan_123
 *
 * @tips     1tab = 4spaces
 */
/****************System Include*************************/
#include <string.h>
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"

/****************Task Include*************************/
#include "app_air_task.h"

/****************BSP Include*************************/
#include "bsp_airquality_driver.h"
#include "bsp_pm2.5_driver.h"
#include "bsp_usart_driver.h"
#include "bsp_esp8266_driver.h"

/****************Semaphore Init*************************/

/****************Queue Init*************************/
extern QueueHandle_t xAirDataQueue;

/*wifi串口数据*/
extern unsigned char esp8266_rx_buffer[ESP8266_RX_BUFFER_SIZE];
/*用于判断串口接收是否完成*/
extern unsigned short esp8266_cnt, esp8266_cntPre;


char rx_buffer_air[20]; // 接收缓冲区 TVCO CO2 HCHO
char rx_buffer_pm25[5]; // 接收缓冲区 PM2.5
airData_t airData;      // 空气质量数据

/*
 * @brief   将从串口接收到的数据发送给 wifi_task
 * @param   *pvParameters 任务创建时传递的参数
 * @retval  None
 */
void air_task(void *pvParameters)
{
    memset(rx_buffer_air, 0, sizeof(rx_buffer_air));
    memset(rx_buffer_pm25, 0, sizeof(rx_buffer_pm25));
    Airquality_rev_data((uint8_t *)rx_buffer_air, sizeof(rx_buffer_air));
    pm25_rev_data((uint8_t *)rx_buffer_pm25, sizeof(rx_buffer_pm25));

    /* 等待 WiFi 准备就绪 */
    while (1)
    {

        if (NULL != xAirDataQueue)
        {
            xQueueSend(xAirDataQueue, &airData, pdMS_TO_TICKS(100));
            //                DEBUG_LOG("Air 发送数据: TVOC=%.2f, CO2=%.2f, HCHO=%.2f, PM25=%d\n",
            //                          airData.TVOC, airData.CO2, airData.HCHO, airData.PM25);
            memset(rx_buffer_air, 0, sizeof(rx_buffer_air));
            memset(rx_buffer_pm25, 0, sizeof(rx_buffer_pm25));
        }

        vTaskDelay(pdMS_TO_TICKS(3000));
    }
}

/*串口空闲中断回调函数*/
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
    taskENTER_CRITICAL();
    if (huart->Instance == UART4)
    {
        airData.TVOC = ((float)rx_buffer_air[2] * 256 + (float)rx_buffer_air[3]) * 0.01;
        airData.HCHO = ((float)rx_buffer_air[4] * 256 + (float)rx_buffer_air[5]) * 0.01;
        airData.CO2 = (float)rx_buffer_air[6] * 256 + (float)rx_buffer_air[7];
        Airquality_rev_data((uint8_t *)rx_buffer_air, sizeof(rx_buffer_air)); // 再次启动接收
    }
    if (huart->Instance == USART6)
    {
        airData.PM25 = (int)rx_buffer_pm25[1] * 128 + (int)rx_buffer_pm25[2];

        pm25_rev_data((uint8_t *)rx_buffer_pm25, sizeof(rx_buffer_pm25)); // 再次启动接收
    }
    taskEXIT_CRITICAL();
}

/*串口错误回调函数*/
void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == UART4)
    {
        DEBUG_LOG("UART4 错误: %d\n", huart->ErrorCode);
        HAL_UART_Abort(huart);
        Airquality_rev_data((uint8_t *)rx_buffer_air, sizeof(rx_buffer_air));
    }
    if (huart->Instance == USART6)
    {
        DEBUG_LOG("USART6 错误: %d\n", huart->ErrorCode);
        HAL_UART_Abort(huart);
        pm25_rev_data((uint8_t *)rx_buffer_pm25, sizeof(rx_buffer_pm25));
    }
}
