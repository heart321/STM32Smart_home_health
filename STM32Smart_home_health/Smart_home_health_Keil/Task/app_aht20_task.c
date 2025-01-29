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
#include "FreeRTOS.h"
#include "task.h"

#include "app_aht20_task.h"

#include "bsp_aht20_driver.h"
#include "bsp_usart_driver.h"


/*
 * @brief   测量环境温湿度数据
 * @param   *pvParameters 任务创建时传递的参数
 * @retval  None
 */
void aht20_task(void *pvParameters)
{

	float temp,humi;
	
	aht20_init();
	
	while(1)
	{
		aht20_read(&temp,&humi);
		//printf("温度：%.2fC 湿度：%.2f%%\r\n",temp,humi);
		vTaskDelay(5000);

	}

}
