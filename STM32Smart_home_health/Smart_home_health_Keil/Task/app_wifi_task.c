/*
 * @file      app_wifi_task.c
 *
 * @brief     连接wifi的任务
 *
 * @version   V1.0
 *
 * @date     2025/1/18 星期六
 *
 * @auther   chenxuan_123
 *
 * @tips     1tab = 4spaces
 */

/****************System_Include***********************/
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

/****************BSP_Include*************************/
#include "bsp_esp8266_driver.h"
#include "bsp_led_driver.h"
#include "bsp_usart_driver.h"
#include "app_wifi_task.h"
#include "Cloud.h"


/*
 * @brief   连接wifi mqtt服务器 订阅 发布
 * @param   *pvParameters 任务创建时传递的参数
 * @retval  None
 */
void wifi_connect_task(void *pvParameters)
{
    wifi_usart2_Init();

    HAL_StatusTypeDef wifi_status = HAL_ERROR;
    ESP8266_StatusTypeDef_t esp8266_status = ESP8266_ERROR; // wifi连接状态
    Cloud_Status_t cloud_statu; // mqtt订阅状态

    char PUB_BUF[256];                       // 要上传的数据
    const char *devSubTopic[] = {"cx/123S"}; // 订阅的Topic
    const char devPubTopic[] = {"cx/123P"};  // 发布的Topic
	unsigned char *dataPtr = NULL;

    
    esp8266_status = ESP8266_Connect(); // 连接WiFi

    while (1)
    {
		/*连接wifi 服务器 订阅Topic*/
        if (HAL_ERROR == wifi_status)
        {
            if (ESP8266_OK == esp8266_status)
            {
                // wifi led 快闪3次
#if DEBUG
                DEBUG_LOG("WiFi连接成功!\n");

#endif
                if (0 == Cloud_DevLink()) // 连接MQTT Broker服务器
                {

                    cloud_statu = Cloud_Subscribe(devSubTopic, 1);
                    // 订阅Topic成功
                    if (0 == cloud_statu)
                    {
#if DEBUG
                        DEBUG_LOG("订阅成功:%s\n", *devSubTopic);
#endif
                        wifi_status = HAL_OK;
                    }
                }
            }
        } // wifi_status = HAL_OK
		
		
		dataPtr = ESP8266_GetIPD(3);
		if(dataPtr != NULL)
		{
			Cloud_RevPro(dataPtr);
		}
		
        vTaskDelay(1000); 
    }
}
