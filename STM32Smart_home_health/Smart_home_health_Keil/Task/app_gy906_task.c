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
#include "freertos_demo.h"
#include "app_gy906_task.h"

/****************BSP Include*************************/
#include "bsp_gy906_driver.h"
#include "bsp_usart_driver.h"

/****************Semaphore Init*************************/


/****************Queue Init*************************/
extern QueueHandle_t xSensorDataQueue;

/*
 * @brief   测量体温的任务
 * @param   *pvParameters 任务创建时传递的参数
 * @retval  None
 */
void gy906_task(void *pvParameters)
{
    SensorData_t data = {
        .type = PEOPLE_TEMP
    };
    uint8_t gy_time = 0;

    while (1)
    {
        gy_time++;
        data.people_temp = readObjectTemp();

        if (3 < gy_time && NULL != xSensorDataQueue)
        {
			if(34 < data.people_temp && 40 > data.people_temp)
			{
				if(pdTRUE == xQueueSend(xSensorDataQueue, &data.people_temp, portMAX_DELAY))
				{
					gy_time = 0;
				}
			}
			printf("%.2f\n",data.people_temp);
        }

        vTaskDelay(pdMS_TO_TICKS(500));
    }
}
