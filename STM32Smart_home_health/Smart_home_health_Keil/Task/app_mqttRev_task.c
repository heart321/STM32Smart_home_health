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
#include "main.h"
/****************Task Include*************************/
#include "app_mqttRev_task.h"

/****************BSP Include*************************/
#include "bsp_esp8266_driver.h"
#include "bsp_sg90_driver.h"
#include "bsp_relay_driver.h"
#include "bsp_wuhua_driver.h"
#include "bsp_drv8833_driver.h"
#include "bsp_usart_driver.h"
#include "cJSON.h"

/****************Semaphore Init*************************/

/****************Queue Init*************************/

/****************BSP Init*************************/
Relay_t Window =
    {
        .port = GPIOC,
        .pin = GPIO_PIN_1

};

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
        //printf("rev task………… \n");
        ptrIPD = ESP8266_GetIPD(200);
        if (ptrIPD != NULL)
        {
            Cloud_RevPro(ptrIPD);
            ESP8266_Clear(); // 处理完后清空缓冲区
        }
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

void revData_Json(cJSON *json)
{
    if (!json) {
        DEBUG_LOG("JSON指针无效\n");
        return;
    }

    cJSON *json_value;
    /*sg90*/
    json_value = cJSON_GetObjectItem(json, "Door");
    if (json_value && json_value->valueint == 1)
    {
        sg90_set_angle(180);
    }
    else
    {
        sg90_set_angle(0);
    }
    /*fenshang*/
    json_value = cJSON_GetObjectItem(json, "Fenshang");
    if (json_value && json_value->valueint == 1)
    {
        drv8833_motor_direction(reversal);
    }
    else
    {
        drv8833_motot_stop();
    }
    /*Window*/
    json_value = cJSON_GetObjectItem(json, "Window");
    if (json_value && json_value->valueint == 1)
    {
        relay_on(Window);
    }
    else
    {
        relay_off(Window);
    }
    /*Wuhua*/
    json_value = cJSON_GetObjectItem(json, "Wuhua");
    if (json_value && json_value->valueint == 1)
    {
        atomizer_on();
    }
    else
    {
        atomizer_off();
    }
}
