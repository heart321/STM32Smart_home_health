/*
 * @file      app_aht20_task.c
 *
 * @brief     测量环境温湿度数据
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

/****************Task Include*************************/
#include "app_aht20_task.h"

/****************BSP Include*************************/
#include "bsp_aht20_driver.h"
#include "bsp_usart_driver.h"

/****************Semaphore Init*************************/
extern SemaphoreHandle_t xAht20ReadySemaphore;


/****************Queue Init*************************/
extern QueueHandle_t xWeatherDataQueue;

/*
 * @brief   测量环境温湿度数据
 * @param   *pvParameters 任务创建时传递的参数
 * @retval  None
 */
void aht20_task(void *pvParameters)
{
	weatherData_t weatherData;
	
	aht20_init();
	
	/*等待wifi准备就绪*/
	if(pdTRUE == xSemaphoreTake(xAht20ReadySemaphore,portMAX_DELAY))
	{

		while(1)
		{
			aht20_read(&weatherData.temperature,&weatherData.humidity);
			if(NULL != xWeatherDataQueue)
			{
				xQueueSend(xWeatherDataQueue,&weatherData,pdMS_TO_TICKS(10));

			}
			// printf("温度：%.2fC 湿度：%.2f%%\r\n",
			// 	weatherData.temperature,weatherData.humidity);
			
//			vTaskDelay(5 * 60 * 1000);
			vTaskDelay(5000);
		}
	}


}

