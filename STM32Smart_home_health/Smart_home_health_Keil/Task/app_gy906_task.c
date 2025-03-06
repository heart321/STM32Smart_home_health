/*
 * @file      app_gy906_task.c
 *
 * @brief     测量体温
 *
 * @version   V1.0
 *
 * @date     2025/3/2 星期天
 *
 * @auther   chenxuan_123
 *
 * @tips     1tab = 4spaces
 */
/****************System Include*************************/
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"

/****************Task Include*************************/
#include "app_gy906_task.h"

/****************BSP Include*************************/
#include "bsp_gy906_driver.h"
#include "bsp_usart_driver.h"

/****************Semaphore Init*************************/
extern SemaphoreHandle_t xGy906ReadySemaphore;

/****************Queue Init*************************/
extern QueueHandle_t xTemperatureDataQueue;

/*
 * @brief   测量体温的任务
 * @param   *pvParameters 任务创建时传递的参数
 * @retval  None
 */
void gy906_task(void *pvParameters)
{
    /*测量的温度变量*/
    temperatureData_t temperatureData;

    /*等待wifi连接成功 发送体温数据 */

    while (1)
    {
        if (xSemaphoreTake(xGy906ReadySemaphore, portMAX_DELAY) == pdTRUE)
        {
            temperatureData.people_temp = readObjectTemp();
            temperatureData.room_temp = 0.0;

            if (NULL != xTemperatureDataQueue)
            {
                xQueueSend(xTemperatureDataQueue, &temperatureData.people_temp, pdMS_TO_TICKS(100));
            }
        }

        vTaskDelay(pdMS_TO_TICKS(3000));
    }
}
