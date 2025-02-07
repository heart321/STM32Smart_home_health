/*
 * @file      app_wifi_task.c
 *
 * @brief     连接wifi的任务 发送数据
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

/****************Task_Include*************************/
#include "app_aht20_task.h"
#include "app_air_task.h"
#include "app_lm2904_task.h"

/****************Semaphore Init*************************/
extern SemaphoreHandle_t xAht20ReadySemaphore;
extern SemaphoreHandle_t xLm2904ReadySemaphore;
extern SemaphoreHandle_t xAirReadySemaphore;

/****************Queue Init*************************/
extern QueueHandle_t xWeatherDataQueue;
extern QueueHandle_t xAirDataQueue;
extern QueueHandle_t xDbDataQueue;


/*
* @brief   连接wifi mqtt服务器 订阅 发布 将数据打包发送给云服务器
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

    /*aht20数据*/
    weatherData_t receiveData_aht20;
    /*空气质量数据*/
    airData_t receiveData_air;
    /*分贝数据*/
    dbData_t receiveData_db;


    
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
						wifi_status = HAL_OK;
						xSemaphoreGive(xAht20ReadySemaphore); // 释放AHt20信号量
                        xSemaphoreGive(xAirReadySemaphore);     // 释放空气质量检测的信号量
                        xSemaphoreGive(xLm2904ReadySemaphore);  //释放lm2904的信号量
#if DEBUG

                        DEBUG_LOG("订阅成功:%s\n", *devSubTopic);
                        DEBUG_LOG("Wifi连接/订阅成功,释放信号量\n");
#endif
                    }
                }
            }
        } // wifi_status = HAL_OK
		

		/*接收数据 使用非阻塞的方式接收 防止接收频率一致*/
        /*1. 接收AHT20的数据 */
        if(pdTRUE == xQueueReceive(xWeatherDataQueue,&receiveData_aht20,pdMS_TO_TICKS(100)))
        {
#if DEBUG
            DEBUG_LOG("wifi_task 温度：%.2fC 湿度：%.2f%%\r\n",
		 	    receiveData_aht20.temperature,receiveData_aht20.humidity);
#endif
        }

        /*2. 接收空气质量数据 TVCO CO2 HCHO pm2.5 */
        if(pdTRUE == xQueueReceive(xAirDataQueue,&receiveData_air,pdMS_TO_TICKS(100)))
        {
#if DEBUG
            DEBUG_LOG("wifi_task TVOC:%.2f CO2:%.2f HCHO:%.2f pm2.5:%d ug/m3\r\n",
			receiveData_air.TVOC,receiveData_air.CO2,receiveData_air.HCHO,receiveData_air.PM25);
#endif
        }

		/*3. 接收db分贝数据*/
        if(pdTRUE == xQueueReceive(xDbDataQueue,&receiveData_db.db_value,pdMS_TO_TICKS(100)))
        {
#if DEBUG
            DEBUG_LOG("wifi_task 分贝值:%.2f\r\n",receiveData_db.db_value);
#endif
        }

		// 接收重新创建一个任务
//		dataPtr = ESP8266_GetIPD(3);
//		if(dataPtr != NULL)
//		{
//			Cloud_RevPro(dataPtr);
//		}
		
        vTaskDelay(1000); 
    }
}
