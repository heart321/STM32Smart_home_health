/****************System_Include*********************/
#include "freertos_demo.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"

/****************Task_Include***********************/
#include "app_wifi_task.h"
#include "app_aht20_task.h"
#include "app_air_task.h"
#include "app_lm2904_task.h"

/****************BSP_Include***********************/
#include "bsp_usart_driver.h"

/****************Semaphore_Init*********************/
/*aht20准备发送数据的的信号量*/
SemaphoreHandle_t xAht20ReadySemaphore = NULL;
/*Air准备发送数据的的信号量*/
SemaphoreHandle_t xAirReadySemaphore = NULL;
/*lm2904准备发送数据的的信号量*/
SemaphoreHandle_t xLm2904ReadySemaphore = NULL;


/****************Queue_Init*********************/
/*aht20 发送数据队列*/
QueueHandle_t xWeatherDataQueue = NULL;

/*空气质量 发送数据队列 */
QueueHandle_t xAirDataQueue = NULL;

/*lm2904 分贝数据发送队列*/
QueueHandle_t xDbDataQueue = NULL;


/****************Task_Init**************************/
/*start_task*/
#define START_TASK_DEPTH 128
#define START_TASK_PRIORITY 2
TaskHandle_t start_task_handle = NULL;

/*wifi_task*/
#define WIFI_TASK_DEPTH 128
#define WIFI_TASK_PRIORITY 24
TaskHandle_t wifi_task_handle = NULL;

/*aht20_task*/
#define AHT20_TASK_DEPTH 128
#define AHT20_TASK_PRIORITY 3
TaskHandle_t aht20_task_handle = NULL;

/*air_task*/
#define AIR_TASK_DEPTH 128
#define AIR_TASK_PRIORITY 4
TaskHandle_t air_task_handle = NULL;

/*lm2904 噪声 task*/
#define LM2904_TASK_DEPTH 128
#define LM2904_TASK_PRIORITY 5
TaskHandle_t lm2904_task_handle = NULL;




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
    /*1. aht20准备发送数据的的信号量*/
	xAht20ReadySemaphore = xSemaphoreCreateBinary();
	
	/*2. air准备发送数据的的信号量*/
	xAirReadySemaphore = xSemaphoreCreateBinary();
 
    /*3. lm2904准备发送数据的的信号量*/
	xLm2904ReadySemaphore = xSemaphoreCreateBinary();

	
	
	
	/*创建消息队列*/
	/*1. ath20发送数据消息队列*/
	xWeatherDataQueue = xQueueCreate(3,sizeof(weatherData_t));
	
    /*2. air发送数据消息队列 */
    xAirDataQueue = xQueueCreate(10,sizeof(airData_t));

    /*3. lm2904发送数据消息队列*/
    xDbDataQueue = xQueueCreate(2,sizeof(dbData_t));

    /*创建其他任务*/
    /*1. wifi connectt*/
    xTaskCreate(
        (TaskFunction_t)wifi_connect_task,
        (char *)"wifi_connect",
        (configSTACK_DEPTH_TYPE)WIFI_TASK_DEPTH,
        (void *)NULL,
        (UBaseType_t)WIFI_TASK_PRIORITY,
        (TaskHandle_t *)&wifi_task_handle);
	
	/*2. aht20 task*/
	xTaskCreate(
        (TaskFunction_t)aht20_task,
        (char *)"aht20_task",
        (configSTACK_DEPTH_TYPE)AHT20_TASK_DEPTH,
        (void *)NULL,
        (UBaseType_t)AHT20_TASK_PRIORITY,
        (TaskHandle_t *)&aht20_task_handle);
		
	/*3. air task*/
	xTaskCreate(
        (TaskFunction_t)air_task,
        (char *)"air_task",
        (configSTACK_DEPTH_TYPE)AIR_TASK_DEPTH,
        (void *)NULL,
        (UBaseType_t)AIR_TASK_PRIORITY,
        (TaskHandle_t *)&air_task_handle);	
	
    /*4. aht20 task*/
	xTaskCreate(
        (TaskFunction_t)lm2904_task,
        (char *)"aht20_task",
        (configSTACK_DEPTH_TYPE)LM2904_TASK_DEPTH,
        (void *)NULL,
        (UBaseType_t)LM2904_TASK_PRIORITY,
        (TaskHandle_t *)lm2904_task_handle);


    /*增加100ms延时*/
    vTaskDelay(pdMS_TO_TICKS(100));

    /*创建任务完成删除任务*/
    vTaskDelete(NULL);

	/*退出临界区*/
    taskEXIT_CRITICAL();
}

/* 信号量状态监控 */
//void vApplicationIdleHook(void)
//{
//    if(NULL != xWifiReadySemaphore) {
//        UBaseType_t count = uxSemaphoreGetCount(xWifiReadySemaphore);

//        DEBUG_LOG("信号量状态：%ld\n",count);

//    }
//}
