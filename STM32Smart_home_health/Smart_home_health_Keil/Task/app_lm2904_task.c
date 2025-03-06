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

/****************Task Include*************************/
#include "app_lm2904_task.h"

/****************BSP Include*************************/
#include "bsp_lm2904_driver.h"
#include "bsp_usart_driver.h"

/****************Semaphore Init*************************/
extern SemaphoreHandle_t xLm2904ReadySemaphore;

/****************Queue Init*************************/
extern QueueHandle_t xDbDataQueue;

/*
 * @brief   测量噪声的db
 * @param   *pvParameters 任务创建时传递的参数
 * @retval  None
 */
void lm2904_task(void *pvParameters)
{
    dbData_t db_value = {0}; // 初始化结构体

    /**等待wifi连接完成 */

    while (1)
    {
        if (pdTRUE == xSemaphoreTake(xLm2904ReadySemaphore, portMAX_DELAY))
        {
            // 开启 ADC 转换，采集单个样本
            lm2904_start(&db_value.adc_value, 1); // 只采集 1 个样本
            vTaskDelay(pdMS_TO_TICKS(10));        // 等待 DMA 传输完成
            lm2904_stop();

            // 将 ADC 值转换为分贝值
            db_value.db_value = adc_to_db(db_value.adc_value);

            // 发送完整结构体到队列，延长超时时间
            if (NULL != xDbDataQueue)
            {
                if (xQueueSend(xDbDataQueue, &db_value, pdMS_TO_TICKS(100)) != pdTRUE)
                {
#if DEBUG
                    // DEBUG_LOG("发送分贝数据到队列失败\n");
#endif
                }
            }
        }

        vTaskDelay(pdMS_TO_TICKS(3000));
    }
}
