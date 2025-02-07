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

/****************Semaphore Init*************************/
extern SemaphoreHandle_t xAirReadySemaphore;

/****************Queue Init*************************/
extern QueueHandle_t xAirDataQueue;

char rx_buffer_air[20]; 	//接收缓冲区 TVCO CO2 HCHO
char rx_buffer_pm25[5]; 	//接收缓冲区 PM2.5
airData_t airData; 			//空气质量数据

/*
 * @brief   将从串口接收到的数据发送给 wifi_task
 * @param   *pvParameters 任务创建时传递的参数
 * @retval  None
 */
void air_task(void *pvParameters)
{
	Airquality_usart4_init();
	pm25_usart6_init();
	
	memset(rx_buffer_air,0,sizeof(rx_buffer_air));
	memset(rx_buffer_pm25,0,sizeof(rx_buffer_pm25));
	

	Airquality_rev_data((uint8_t*)rx_buffer_air,sizeof(rx_buffer_air));
	pm25_rev_data((uint8_t*)rx_buffer_pm25,sizeof(rx_buffer_pm25));

	

	/*等待wifi准备就绪*/
	if(pdTRUE == xSemaphoreTake(xAirReadySemaphore,portMAX_DELAY))
	{
		while(1)
		{
			if(NULL != xAirDataQueue)
			{
				// 将空气数据发送给wifi task 传感器损坏 TODO
				xQueueSend(xAirDataQueue,&airData,pdMS_TO_TICKS(10));
				//DEBUG_LOG("Air发送数据成功！\n");
			}
			vTaskDelay(pdMS_TO_TICKS(2000));
		}
	}


}

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
	if(huart->Instance == UART4)
	{
		// 空气质量传感器的数据
		airData.TVOC = ((float)rx_buffer_air[2]*256+(float)rx_buffer_air[3])*0.01;
		airData.HCHO = ((float)rx_buffer_air[4]*256+(float)rx_buffer_air[5])*0.01;
		airData.CO2 = (float)rx_buffer_air[6]*256+(float)rx_buffer_air[7];
		Airquality_rev_data((uint8_t*)rx_buffer_air, sizeof(rx_buffer_air)); 
	}
	if(huart->Instance == USART6)
	{
		// PM2.5数据
		airData.PM25 = (int)rx_buffer_pm25[1] * 128 + (int)rx_buffer_pm25[2];
		pm25_rev_data((uint8_t*)rx_buffer_pm25,sizeof(rx_buffer_pm25));
		
	}
	//DEBUG_LOG("TVOC:%.2f HCHO:%.2f CO2:%.2f PM25:%d\n",airData.TVOC,airData.HCHO,airData.CO2,airData.PM25);

}

