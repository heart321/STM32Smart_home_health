/*
 * @file      app_max30102_task.c
 *
 * @brief     获取MAX30102心率血氧数据
 *
 * @version   V1.0
 *
 * @date     2025/3/18 星期二
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
#include "freertos_demo.h"
#include "app_max30102_task.h"

/****************BSP Include*************************/
#include "bsp_max30102_driver.h"
#include "bsp_usart_driver.h"

/****************Semaphore Init*************************/

/****************Queue Init*************************/
extern QueueHandle_t xSensorDataQueue;

/****************BSP Init*************************/
#define MAX_BRIGHTNESS 255
#define INTERRUPT_REG 0X00

uint32_t aun_ir_buffer[500];
int32_t n_ir_buffer_length;
uint32_t aun_red_buffer[500];
int32_t n_sp02;
int8_t ch_spo2_valid;
int32_t n_heart_rate;
int8_t ch_hr_valid;

uint32_t un_min, un_max, un_prev_data;
int i;
int32_t n_brightness;
float f_temp;
uint8_t temp[6];
uint8_t finger_detected = 0; // 标记是否有手指


/*
 * @brief   采集心率血氧数据
 * @param   *pvParameters 任务创建时传递的参数
 * @retval  None
 */
void max30102_task(void *pvParameters)
{

    SensorData_t data = {
        .type = MAX30102
    };
    uint8_t max30102_time = 0;

    un_min = 0x3FFFF;
    un_max = 0;

    n_ir_buffer_length = 500;

    for (i = 0; i < n_ir_buffer_length; i++)
    {
        while (MAX30102_INT == 1);
        max30102_FIFO_ReadBytes(REG_FIFO_DATA, temp);
        aun_red_buffer[i] = (long)((long)((long)temp[0] & 0x03) << 16) | (long)temp[1] << 8 | (long)temp[2];
        aun_ir_buffer[i] = (long)((long)((long)temp[3] & 0x03) << 16) | (long)temp[4] << 8 | (long)temp[5];

        if (un_min > aun_red_buffer[i])
            un_min = aun_red_buffer[i];
        if (un_max < aun_red_buffer[i])
            un_max = aun_red_buffer[i];
    }
    un_prev_data = aun_red_buffer[i];

    maxim_heart_rate_and_oxygen_saturation(aun_ir_buffer, n_ir_buffer_length, aun_red_buffer, &n_sp02, &ch_spo2_valid, &n_heart_rate, &ch_hr_valid);

    
    while (1)
    {
        max30102_time++;

        for (i = 100; i < 500; i++)
        {
            aun_red_buffer[i - 100] = aun_red_buffer[i];
            aun_ir_buffer[i - 100] = aun_ir_buffer[i];

            if (un_min > aun_red_buffer[i])
                un_min = aun_red_buffer[i];
            if (un_max < aun_red_buffer[i])
                un_max = aun_red_buffer[i];
        }

        finger_detected = 0; // 重置手指检测标志
        for (i = 400; i < 500; i++)
        {
            un_prev_data = aun_red_buffer[i - 1];
            while (MAX30102_INT == 1);
            max30102_FIFO_ReadBytes(REG_FIFO_DATA, temp);
            aun_red_buffer[i] = (long)((long)((long)temp[0] & 0x03) << 16) | (long)temp[1] << 8 | (long)temp[2];
            aun_ir_buffer[i] = (long)((long)((long)temp[3] & 0x03) << 16) | (long)temp[4] << 8 | (long)temp[5];


            // 判断信号幅度是否足够（动态阈值）
            uint32_t signal_range = un_max - un_min;
            if (signal_range > 10000 && aun_red_buffer[i] > 50000 && aun_ir_buffer[i] > 50000)
            {
                finger_detected = 1;
            }
            else
            {
                finger_detected = 0;
                MAX30102_Clear_FIFO();
                data.dis_hr = 0;
                data.dis_spo2 = 0;
                //printf("无手指检测\r\n");
                break; // 如果检测到无手指，跳出采集循环
            }

            if (aun_red_buffer[i] > un_prev_data)
            {
                f_temp = aun_red_buffer[i] - un_prev_data;
                f_temp /= (un_max - un_min);
                f_temp *= MAX_BRIGHTNESS;
                n_brightness -= (int)f_temp;
                if (n_brightness < 0)
                    n_brightness = 0;
            }
            else
            {
                f_temp = un_prev_data - aun_red_buffer[i];
                f_temp /= (un_max - un_min);
                f_temp *= MAX_BRIGHTNESS;
                n_brightness += (int)f_temp;
                if (n_brightness > MAX_BRIGHTNESS)
                    n_brightness = MAX_BRIGHTNESS;
            }
        }

        if (finger_detected)
        {
            maxim_heart_rate_and_oxygen_saturation(aun_ir_buffer, n_ir_buffer_length, aun_red_buffer, &n_sp02, &ch_spo2_valid, &n_heart_rate, &ch_hr_valid);

            if (ch_hr_valid == 1 && n_heart_rate < 120)
            {
                data.dis_hr = n_heart_rate - 15;
                data.dis_spo2 = n_sp02;
            }
        }
        else
        {
            data.dis_hr = 0;
            data.dis_spo2 = 0;
        }
        

        // 使用消息队列发送数据
        if(5 < max30102_time &&  NULL != xSensorDataQueue)
        {
			if(0 != data.dis_hr && 0 != data.dis_spo2)
			{
				if(pdTRUE == xQueueSend(xSensorDataQueue,&data,500))
				{
					max30102_time = 0;
				}
			}
        }
        
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}
