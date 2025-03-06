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
#include "app_revData_task.h"

/****************BSP Include*************************/
#include "bsp_esp8266_driver.h"
#include "bsp_sg90_driver.h"

/****************Semaphore Init*************************/
extern SemaphoreHandle_t xRevDataReadySemaphore;

/****************Queue Init*************************/

/*
 * @brief   接收MQTT返回的数据并进行处理
 * @param   *pvParameters 任务创建时传递的参数
 * @retval  None
 */
void rev_data_task(void *pvParameters)
{
    /*服务器返回的数据 */
    unsigned char *ptrIPD = NULL;

    while (1)
    {
        if (xSemaphoreTake(xRevDataReadySemaphore, portMAX_DELAY) == pdTRUE)
        {
            ptrIPD = ESP8266_GetIPD(200);
            if (ptrIPD != NULL)
            {
                /*处理数据 */
                Cloud_RevPro(ptrIPD);
            }
        }

        vTaskDelay(pdMS_TO_TICKS(10));
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
