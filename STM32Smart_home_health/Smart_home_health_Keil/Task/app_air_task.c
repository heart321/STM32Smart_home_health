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
#include "main.h"
/****************Task Include*************************/
#include "app_air_task.h"

/****************BSP Include*************************/
#include "bsp_airquality_driver.h"
#include "bsp_pm2.5_driver.h"
#include "bsp_usart_driver.h"
#include "bsp_esp8266_driver.h"
#include "bsp_iwdog_driver.h"
#include "bsp_oled_driver.h"

/****************Semaphore Init*************************/
extern SemaphoreHandle_t xAirQualitySemaphore;
extern SemaphoreHandle_t xPM25Semaphore;
/****************Queue Init*************************/
extern QueueHandle_t xSensorDataQueue;

/*wifi串口数据*/
extern unsigned char esp8266_rx_buffer[ESP8266_RX_BUFFER_SIZE];
/*用于判断串口接收是否完成*/
extern unsigned short esp8266_cnt, esp8266_cntPre;

static TickType_t last_send_time = 0; // 上次发送的时间

uint8_t rx_buffer_air[20]; // 接收缓冲区 TVCO CO2 HCHO
uint8_t rx_buffer_pm25[5]; // 接收缓冲区 PM2.5

SensorData_t data = {
    .type = AIR};
// uint8_t air_time = 0;

// 添加时间计数器
// static uint32_t sensor_timer = 0;
// #define SENSOR_READ_INTERVAL 30
/*
 * @brief   将从串口接收到的数据发送给 wifi_task
 * @param   *pvParameters 任务创建时传递的参数
 * @retval  None
 */
void air_task(void *pvParameters)
{

    
    memset(rx_buffer_air, 0, sizeof(rx_buffer_air));
    memset(rx_buffer_pm25, 0, sizeof(rx_buffer_pm25));

    Airquality_rev_data(rx_buffer_air, sizeof(rx_buffer_air));
    pm25_rev_data(rx_buffer_pm25, sizeof(rx_buffer_pm25));
    // 初始关闭串口接收
    //    pm25_rev_data_stop();
    //    Airquality_rev_data_stop();

    /* 等待 WiFi 准备就绪 */
    while (1)
    {
        //printf("air task………… \n");
				OLED_Showdecimal(94,4,data.sensor_values.air.TVOC,1,3,12,0);
				OLED_Showdecimal(94,5,data.sensor_values.air.HCHO,1,3,12,0);
				OLED_ShowNum(94, 6, data.sensor_values.air.CO2, 3, 12, 0);
				OLED_ShowNum(100, 7, data.sensor_values.air.PM25, 3, 12, 0);
        vTaskDelay(pdMS_TO_TICKS(5000)); // 每秒检查一次
    }
}

/*串口空闲中断回调函数*/
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
    BaseType_t xTaskWork = pdFALSE;
    TickType_t current_time = xTaskGetTickCountFromISR();
    if (huart->Instance == UART4)
    {
        data.sensor_values.air.TVOC = ((float)rx_buffer_air[2] * 256 + (float)rx_buffer_air[3]) * 0.01;
        data.sensor_values.air.HCHO = ((float)rx_buffer_air[4] * 256 + (float)rx_buffer_air[5]) * 0.01;
        data.sensor_values.air.CO2 = (float)rx_buffer_air[6] * 256 + (float)rx_buffer_air[7];

        memset(rx_buffer_air, 0, sizeof(rx_buffer_air));
        Airquality_rev_data(rx_buffer_air, sizeof(rx_buffer_air));
    }
    else if (huart->Instance == USART6)
    {
        data.sensor_values.air.PM25 = ((int)rx_buffer_pm25[1] & 0x7F) * 128 + ((int)rx_buffer_pm25[2] & 0x7F);
        memset(rx_buffer_pm25, 0, sizeof(rx_buffer_pm25));
        pm25_rev_data(rx_buffer_pm25, sizeof(rx_buffer_pm25));
    }

    if ((current_time - last_send_time) >= SEND_INTERVAL)
    {
        // 从中断上下文将传感器数据 (data) 发送到 xSensorDataQueue 队列
        // xTaskWork 会被置位，如果队列操作需要唤醒更高优先级的任务
        xQueueSendFromISR(xSensorDataQueue, &data, &xTaskWork);

			
        last_send_time = current_time;
    }

    // 根据 xTaskWork 的值决定是否需要进行任务切换（让更高优先级的任务运行）
    // 这是在 FreeRTOS 中断服务例程中确保调度器正确工作的关键步骤
    portYIELD_FROM_ISR(xTaskWork);
}

/*串口错误回调函数*/
void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == UART4 || huart->Instance == USART6)
    {
        // 清除过载错误标志
        if (huart->ErrorCode & HAL_UART_ERROR_ORE)
        {
            __HAL_UART_CLEAR_FLAG(huart, UART_FLAG_ORE);
        }
        // 重新启动接收
        HAL_UARTEx_ReceiveToIdle_IT(huart, &huart->pRxBuffPtr[0], 1);
    }
}
