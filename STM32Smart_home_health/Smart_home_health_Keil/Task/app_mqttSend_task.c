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

/****************BSP_Include*************************/
#include "bsp_esp8266_driver.h"
#include "bsp_led_driver.h"
#include "bsp_usart_driver.h"
#include "bsp_oled_driver.h"
#include "Cloud.h"
#include "Mqttkit.h"


/****************Task_Include*************************/
#include "app_mqttSend_task.h"
#include "app_aht20_task.h"
#include "app_air_task.h"
#include "app_lm2904_task.h"
#include "app_gy906_task.h"


/****************Semaphore Init*************************/

/****************Queue Init*************************/
extern QueueHandle_t xWeatherDataQueue;
extern QueueHandle_t xAirDataQueue;
extern QueueHandle_t xDbDataQueue;
extern QueueHandle_t xTemperatureDataQueue;


ESP8266_StatusTypeDef_t esp8266_status = ESP8266_ERROR;

/*
 * @brief   MQTT 发布主题和数据
 * @param   *pvParameters 任务创建时传递的参数
 * @retval  None
 */
void mqtt_send_task(void *pvParameters) {
    char PUB_BUF[512];
    const char devPubTopic[] = {"smart_home/publish"};
    weatherData_t receiveData_aht20 = {0.0f, 0.0f};
    airData_t receiveData_air = {0.0f, 0.0f, 0.0f, 0};
    dbData_t receiveData_db = {0.0f};
    temperatureData_t temperatureData = {0.0f,0.0f};

    while (1) {
        /* 1. 接收 AHT20 数据 */
        if (xQueueReceive(xWeatherDataQueue, &receiveData_aht20, pdMS_TO_TICKS(500)) == pdTRUE) {
#if DEBUG
//            DEBUG_LOG("wifi_task 温度：%.2fC 湿度：%.2f%%\n",
//                      receiveData_aht20.temperature, receiveData_aht20.humidity);
#endif
        }

        /* 2. 接收空气质量数据 */
        if (xQueueReceive(xAirDataQueue, &receiveData_air, pdMS_TO_TICKS(500)) == pdTRUE) {
#if DEBUG
//		DEBUG_LOG("wifi_task TVOC:%.2f CO2:%.2f HCHO:%.2f pm2.5:%d ug/m3\n",
//				  receiveData_air.TVOC, receiveData_air.CO2, receiveData_air.HCHO, receiveData_air.PM25);
#endif
        }

        /* 3. 接收分贝数据 */
        if (xQueueReceive(xDbDataQueue, &receiveData_db, pdMS_TO_TICKS(500)) == pdTRUE) {
#if DEBUG
//            DEBUG_LOG("wifi_task 分贝值:%.2f\n", receiveData_db.db_value);
#endif
        }

        /*4. 接收体温数据 */
        if(xQueueReceive(xTemperatureDataQueue,&temperatureData,pdMS_TO_TICKS(500) == pdTRUE))
        {
 #if DEBUG
//         DEBUG_LOG("wifi_task 体温:%.2f\n",temperatureData.people_temp);
 #endif
        }


        /* . 打包并发布数据 */
        int len = snprintf(PUB_BUF, sizeof(PUB_BUF),
                           "{\"Temp\":\"%.2f\",\"Humi\":\"%.2f\",\"TVOC\":\"%.2f\",\"CO2\":\"%.1f\",\"HCHO\":\"%.3f\",\"PM25\":\"%d\",\"DB\":\"%.2f\",\"people_temp\":\"%.2f\"}",
                           receiveData_aht20.temperature, receiveData_aht20.humidity,
                           receiveData_air.TVOC, receiveData_air.CO2, receiveData_air.HCHO,
                           receiveData_air.PM25, receiveData_db.db_value,
                           temperatureData.people_temp);
        if (len >= sizeof(PUB_BUF)) {
#if DEBUG
            DEBUG_LOG("PUB_BUF 溢出!\n");
#endif
            continue;
        }

        if (esp8266_status == ESP8266_OK) {
            Cloud_Status_t public_status = Cloud_Publish(devPubTopic, PUB_BUF);
            if (public_status == Cloud_Ok) {
#if DEBUG
                DEBUG_LOG("数据上传成功 \n");
#endif
            } else {
#if DEBUG
                DEBUG_LOG("数据上传失败 \n");
#endif
                esp8266_status = ESP8266_ERROR;
            }
        } else {
#if DEBUG
            DEBUG_LOG("连接断开，跳过发布 \n");
			
#endif
        }


        ESP8266_Clear();
//		char task_buffer[256];
//		vTaskList(task_buffer);
//		DEBUG_LOG("任务状态:\n%s\n", task_buffer);
//		DEBUG_LOG("堆剩余: %u\n", xPortGetFreeHeapSize());
//		DEBUG_LOG("wifi_connect 堆栈剩余: %lu\n", uxTaskGetStackHighWaterMark(NULL));
        vTaskDelay(pdMS_TO_TICKS(3000));
    }
}

/*
 * @brief   初始化 WiFi 和 MQTT 连接
 * @param   None
 * @retval  HAL_OK - 成功, HAL_ERROR - 失败
 */
HAL_StatusTypeDef wifi_mqtt_init(void) {
    
    Cloud_Status_t cloud_statu;

    const char *devSubTopic[] = {"smart_home/subscribe"};

    esp8266_status = ESP8266_Connect();
    if (esp8266_status != ESP8266_OK) {
#if DEBUG
        DEBUG_LOG("WiFi Tcp连接失败!\n");
#endif
        return HAL_ERROR;
    }
#if DEBUG
    DEBUG_LOG("WiFi Tcp连接成功!\n");
#endif

    if (Cloud_DevLink() == 0) {
        cloud_statu = Cloud_Subscribe(devSubTopic, 1);
        if (cloud_statu == Cloud_Ok) {
#if DEBUG
            DEBUG_LOG("订阅成功: %s\n", *devSubTopic);
#endif
            return HAL_OK;
        } else {
#if DEBUG
            DEBUG_LOG("订阅失败!\n");
#endif
            return HAL_ERROR;
        }
    } else {
#if DEBUG
        DEBUG_LOG("MQTT Broker 连接失败!\n");
#endif
        return HAL_ERROR;
    }
}
 