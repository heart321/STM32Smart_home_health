/*
 * @file      app_lm2904_task.c
 *
 * @brief     测量噪声的分贝
 *
 * @version   V1.0
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
    dbData_t db_value;

    lm2904_init();

    /**等待wifi连接完成 */
    if(pdTRUE == xSemaphoreTake(xLm2904ReadySemaphore,portMAX_DELAY))
    {

        while (1)
        {
            // 开启adc转换
            lm2904_start(&db_value.adc_value,sizeof(db_value.adc_value));
            // 将ADC模拟值转换为分贝值
            db_value.db_value = adc_to_db(db_value.adc_value);
            // 关闭ADC转换
            lm2904_stop();

            // 发送数据
            if(NULL != xDbDataQueue)
            {
                xQueueSend(xDbDataQueue,&db_value.db_value,pdMS_TO_TICKS(10));
            }
			
            vTaskDelay(pdMS_TO_TICKS(1000));
        }
        

    }

}
