/*
 * @file      app_lm2904_task.c
 *
 * @brief     测量噪声的分贝
 *
 * @version   V1.2
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
#include "main.h"
/****************Task Include*************************/

#include "app_lm2904_task.h"

/****************BSP Include*************************/
#include "bsp_lm2904_driver.h"
#include "bsp_usart_driver.h"

/****************Semaphore Init*************************/

/****************Queue Init*************************/
extern QueueHandle_t xSensorDataQueue;

/*
 * @brief   测量噪声的db
 * @param   *pvParameters 任务创建时传递的参数
 * @retval  None
 */
void lm2904_task(void *pvParameters)
{
    SensorData_t data = {
        .type = DB};
    uint8_t db_time = 0;

    while (1)
    {
        //printf("lm2904task!! \n");
        // 开启 ADC 转换，采集单个样本
        lm2904_start(&data.sensor_values.db.adc_DB, 1); // 只采集 1 个样本
        vTaskDelay(pdMS_TO_TICKS(10));    // 等待 DMA 传输完成
        lm2904_stop();

        // 将 ADC 值转换为分贝值
        data.sensor_values.db.DB = adc_to_db(data.sensor_values.db.adc_DB);

        db_time++;
        // 发送完整结构体到队列，延长超时时间
        if (6 < db_time && NULL != xSensorDataQueue)
        {
            if (pdTRUE == xQueueSend(xSensorDataQueue, &data, pdMS_TO_TICKS(500)))
            {
                db_time = 0;
            }
        }

        vTaskDelay(pdMS_TO_TICKS(3000));
    }
}
