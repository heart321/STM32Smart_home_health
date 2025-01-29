/****************System_Include*********************/
#include "freertos_demo.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

/****************Task_Include***********************/
#include "app_wifi_task.h"
#include "app_aht20_task.h"

/****************BSP_Include***********************/
#include "bsp_usart_driver.h"

/****************Semaphore_Init*********************/



/****************Task_Init**************************/
/*start_task*/
#define START_TASK_DEPTH 128
#define START_TASK_PRIORITY 1
TaskHandle_t start_task_handle = NULL;

/*wifi_task*/
#define WIFI_TASK_DEPTH 128
#define WIFI_TASK_PRIORITY 2
TaskHandle_t wifi_task_handle = NULL;

/*aht20_task*/
#define AHT20_TASK_DEPTH 128
#define AHT20_TASK_PRIORITY 3
TaskHandle_t aht20_task_handle = NULL;


void start_task(void *pvParameters);

/*
 * @brief FreeRTOS Start
 * @param None
 * @retval None
 */
void freeRTOS_start(void)
{
    /*创建启动函数*/
    xTaskCreate(
        (TaskFunction_t)start_task,
        (char *)"Start_Task",
        (configSTACK_DEPTH_TYPE)START_TASK_DEPTH,
        (void *)NULL,
        (UBaseType_t)START_TASK_PRIORITY,
        (TaskHandle_t *)&start_task_handle);

    /*开启FreeRTOS任务调度*/
    vTaskStartScheduler();
}

/*
 * @brief create start task
 * @param None
 * @retval None
 */
void start_task(void *pvParameters)
{
	/*初始化调试串口*/
	debug_usart1_Init();
	
    /*进入临界区*/
    taskENTER_CRITICAL();

    /*创建信号量*/


    /*创建其他任务*/
    /*wifi connectt*/
    xTaskCreate(
        (TaskFunction_t)wifi_connect_task,
        (char *)"wifi_connect",
        (configSTACK_DEPTH_TYPE)WIFI_TASK_DEPTH,
        (void *)NULL,
        (UBaseType_t)WIFI_TASK_PRIORITY,
        (TaskHandle_t *)&wifi_task_handle);
	
	/*aht20 task*/
	xTaskCreate(
        (TaskFunction_t)aht20_task,
        (char *)"aht20_task",
        (configSTACK_DEPTH_TYPE)AHT20_TASK_DEPTH,
        (void *)NULL,
        (UBaseType_t)AHT20_TASK_PRIORITY,
        (TaskHandle_t *)&aht20_task_handle);
	
	

    /*增加100ms延时*/
    vTaskDelay(pdMS_TO_TICKS(100));

    /*创建任务完成删除任务*/
    vTaskDelete(NULL);

	/*退出临界区*/
    taskEXIT_CRITICAL();
}
