/*
 * @file      app_aht20_task.c
 *
 * @brief     测量环境温湿度数据 10s 发送一次数据
 *
 * @version   V1.0
 *
 * @date     2025/1/27 星期一
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
#include "main.h"

/****************Task Include*************************/
#include "app_aht20_task.h"

/****************BSP Include*************************/
#include "bsp_aht20_driver.h"
#include "bsp_usart_driver.h"
#include "bsp_oled_driver.h"

/****************Semaphore Init*************************/

/****************Queue Init*************************/
extern QueueHandle_t xSensorDataQueue;

/*
 * @brief   测量环境温湿度数据
 * @param   *pvParameters 任务创建时传递的参数
 * @retval  None
 */
void aht20_task(void *pvParameters)
{
	// 定义传感器类型
	SensorData_t data = {
		.type = TEMP_AND_HUMI};

	uint8_t aht20_time = 0;
	while (1)
	{
		aht20_time++;

		if (aht20_time > 21 && NULL != xSensorDataQueue)
		{
			aht20_read(&data.sensor_values.aht20.temperature, &data.sensor_values.aht20.humidity);
			if (0 != data.sensor_values.aht20.humidity && 0 != data.sensor_values.aht20.temperature)
			{
				if (pdTRUE == xQueueSend(xSensorDataQueue, &data, pdMS_TO_TICKS(500)))
				{
					OLED_ShowNum(74, 2, (int)data.sensor_values.aht20.temperature, 2, 12, 0);
					OLED_ShowNum(108, 2, (int)data.sensor_values.aht20.humidity, 2, 12, 0);
					aht20_time = 0;
				}
			}
		}

		vTaskDelay(pdMS_TO_TICKS(1000));
	}
}
