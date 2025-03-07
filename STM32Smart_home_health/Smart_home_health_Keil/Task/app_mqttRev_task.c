/*
 * @file      app_revData_task.c
 *
 * @brief     接收MQTT返回的数据并进行处理
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
#include "app_mqttRev_task.h"

/****************BSP Include*************************/
#include "bsp_esp8266_driver.h"
#include "bsp_sg90_driver.h"

/****************Semaphore Init*************************/

/****************Queue Init*************************/

/*
 * @brief   接收MQTT返回的数据并进行处理
 * @param   *pvParameters 任务创建时传递的参数
 * @retval  None
 */
void mqtt_rev_task(void *pvParameters)
{
    unsigned char *ptrIPD = NULL;

    while (1)
    {
        ptrIPD = ESP8266_GetIPD(1000); // 增加超时到1000ms
        if (ptrIPD != NULL)
        {
            taskENTER_CRITICAL();
            Cloud_RevPro(ptrIPD);
            taskEXIT_CRITICAL();
            ESP8266_Clear(); // 处理完后清空缓冲区
        }
        // 无数据时安静等待，避免频繁轮询
        vTaskDelay(pdMS_TO_TICKS(500)); // 每500ms检查一次
    }
}

void revData_Json(cJSON *json)
{
    cJSON *json_value;
    /*sg90*/
    json_value = cJSON_GetObjectItem(json, "Door");
    if (json_value->valueint == 1)
    {
        sg90_set_angle(180);
        vTaskDelay(pdMS_TO_TICKS(5));
    }
    else
    {
        sg90_set_angle(0);
        vTaskDelay(pdMS_TO_TICKS(5));
    }
}
