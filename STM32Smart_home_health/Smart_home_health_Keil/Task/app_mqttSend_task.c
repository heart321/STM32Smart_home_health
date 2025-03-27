/*
 * @file      app_wifi_task.c
 *
 * @brief     连接wifi的任务 发送数据
 *
 * @version   V1.6
 *
 * @date      2025/1/18 星期六
 *
 * @auther    chenxuan_123
 *
 * @tips      1tab = 4spaces
 */

/****************System_Include***********************/
#include <string.h>
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "main.h"
/****************BSP_Include*************************/
#include "bsp_esp8266_driver.h"
#include "bsp_led_driver.h"
#include "bsp_usart_driver.h"
#include "bsp_oled_driver.h"
#include "Cloud.h"
#include "Mqttkit.h"
#include "bsp_iwdog_driver.h"

/****************Task_Include*************************/
#include "app_mqttSend_task.h"
#include "app_aht20_task.h"
#include "app_air_task.h"
#include "app_lm2904_task.h"
#include "app_gy906_task.h"
#include "app_max30102_task.h"


/****************Semaphore Init*************************/

/****************Queue Init*************************/
extern QueueHandle_t xSensorDataQueue;

ESP8266_StatusTypeDef_t esp8266_status = ESP8266_ERROR;

/*
 * @brief   MQTT 发布主题和数据
 * @param   *pvParameters 任务创建时传递的参数
 * @retval  None
 */
void mqtt_send_task(void *pvParameters)
{
    char PUB_BUF[512];
    const char devPubTopic[] = {"smart_home/publish"};
    SensorData_t rev_data;

    memset(PUB_BUF, 0, sizeof(PUB_BUF));
    while (1)
    {
				/*喂狗*/
				iwdog_refresh();
        //printf("send task………… \n");
				// 检查WiFi和MQTT状态
        if (esp8266_status != ESP8266_OK ) {
            printf("Network disconnected, reconnecting...\n");
            if (wifi_mqtt_init() != HAL_OK) {
                printf("Reconnection failed!\n");
                vTaskDelay(pdMS_TO_TICKS(1000));
                continue;
            }
        }
        if (pdTRUE == xQueueReceive(xSensorDataQueue, &rev_data, 500))
        {
            switch (rev_data.type)
            {
            case TEMP_AND_HUMI:
                sprintf(PUB_BUF, "{\"Temp\":\"%.2f\",\"Humi\":\"%.2f\"}",
                        rev_data.sensor_values.aht20.temperature, rev_data.sensor_values.aht20.humidity);
                Cloud_Publish(devPubTopic, PUB_BUF);
                break;
            case AIR:
                sprintf(PUB_BUF, "{\"TVOC\":\"%.2f\",\"CO2\":\"%d\",\"HCHO\":\"%.3f\",\"PM25\":\"%d\"}",
                        rev_data.sensor_values.air.TVOC, rev_data.sensor_values.air.CO2, rev_data.sensor_values.air.HCHO, rev_data.sensor_values.air.PM25);
                Cloud_Publish(devPubTopic, PUB_BUF);
                break;
            case DB:
                sprintf(PUB_BUF, "{\"DB\":\"%.2f\"}", rev_data.sensor_values.db.DB);
                Cloud_Publish(devPubTopic, PUB_BUF);
                break;
            case PEOPLE_TEMP:
                sprintf(PUB_BUF, "{\"people_temp\":\"%.2f\"}", rev_data.sensor_values.gy906.people_temp);
                Cloud_Publish(devPubTopic, PUB_BUF);
                break;
            case MAX30102:
                sprintf(PUB_BUF, "{\"HR\":\"%.d\",\"SpO2\":\"%.d\"}",
                        rev_data.sensor_values.max30102.heart_rate, rev_data.sensor_values.max30102.spo2);
                Cloud_Publish(devPubTopic, PUB_BUF);
                break;
            default:
                break;
            }
						//printf("mqtt_send_task stack remaining: %lu\n", uxTaskGetStackHighWaterMark(NULL));
            memset(PUB_BUF, 0, sizeof(PUB_BUF));
        }
        vTaskDelay(pdMS_TO_TICKS(400));
    }
}

/*
 * @brief   初始化 WiFi 和 MQTT 连接
 * @param   None
 * @retval  HAL_OK - 成功, HAL_ERROR - 失败
 */
HAL_StatusTypeDef wifi_mqtt_init(void)
{

    Cloud_Status_t cloud_statu;

    const char *devSubTopic[] = {"smart_home/subscribe"};
		
		OLED_Clear();
		OLED_ShowString(0,0,"wifi Connect...",16,0);
    esp8266_status = ESP8266_Connect();
    if (esp8266_status != ESP8266_OK)
    {
#if DEBUG
        DEBUG_LOG("WiFi Tcp连接失败!\n");
#endif
        return HAL_ERROR;
    }
		OLED_Clear();
		OLED_ShowString(0,2,"wifi Success!!",16,0);
#if DEBUG
    DEBUG_LOG("WiFi Tcp连接成功!\n");
#endif

    if (Cloud_DevLink() == 0)
    {
        cloud_statu = Cloud_Subscribe(devSubTopic, 1);
        if (cloud_statu == Cloud_Ok)
        {
#if DEBUG
            DEBUG_LOG("订阅成功: %s\n", *devSubTopic);
#endif
            return HAL_OK;
        }
        else
        {
#if DEBUG
            DEBUG_LOG("订阅失败!\n");
#endif
            return HAL_ERROR;
        }
    }
    else
    {
#if DEBUG
        DEBUG_LOG("MQTT Broker 连接失败!\n");
#endif
        return HAL_ERROR;
    }
}
