/****************System_Include*********************/
#include "freertos_demo.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"
#include "list.h"

/****************Task_Include***********************/
#include "app_wifi_task.h"
#include "app_aht20_task.h"
#include "app_air_task.h"
#include "app_lm2904_task.h"
#include "app_gy906_task.h"
#include "app_revData_task.h"

/****************BSP_Include***********************/
#include "bsp_usart_driver.h"
#include "bsp_aht20_driver.h"
#include "bsp_lm2904_driver.h"
#include "bsp_airquality_driver.h"
#include "bsp_oled_driver.h"
#include "bsp_led_driver.h"
#include "bsp_pm2.5_driver.h"
#include "bsp_motor_driver.h"
#include "bsp_sg90_driver.h"
#include "bsp_led_driver.h"
#include "bsp_relay_driver.h"
#include "bsp_buzzer_driver.h"
#include "bsp_wuhua_driver.h"



/****************Semaphore_Init*********************/
/*aht20准备发送数据的的信号量*/
SemaphoreHandle_t xAht20ReadySemaphore = NULL;
/*Air准备发送数据的的信号量*/
SemaphoreHandle_t xAirReadySemaphore = NULL;
/*lm2904准备发送数据的的信号量*/
SemaphoreHandle_t xLm2904ReadySemaphore = NULL;
/*gy906准备发送数据的信号量*/
SemaphoreHandle_t xGy906ReadySemaphore = NULL;
/*开始接收数据的信号量*/
SemaphoreHandle_t xRevDataReadySemaphore = NULL;

/****************Queue_Init*********************/
/*aht20 发送数据队列*/
QueueHandle_t xWeatherDataQueue = NULL;

/*空气质量 发送数据队列 */
QueueHandle_t xAirDataQueue = NULL;

/*lm2904 分贝数据发送队列*/
QueueHandle_t xDbDataQueue = NULL;

/*gy906 体温数据发送队列*/
QueueHandle_t xTemperatureDataQueue = NULL;


/****************Task_Init**************************/
/*start_task*/
#define START_TASK_DEPTH 512
#define START_TASK_PRIORITY 1
TaskHandle_t start_task_handle = NULL;

/*wifi_task*/
#define WIFI_TASK_DEPTH 512
#define WIFI_TASK_PRIORITY 23
TaskHandle_t wifi_task_handle = NULL;

/*aht20_task*/
#define AHT20_TASK_DEPTH 256
#define AHT20_TASK_PRIORITY 25
TaskHandle_t aht20_task_handle = NULL;

/*air_task*/
#define AIR_TASK_DEPTH 256
#define AIR_TASK_PRIORITY 25
TaskHandle_t air_task_handle = NULL;

/*lm2904 噪声 task*/
#define LM2904_TASK_DEPTH 256
#define LM2904_TASK_PRIORITY 25
TaskHandle_t lm2904_task_handle = NULL;

/*gy906 体温 task*/
#define GY906_TASK_DEPTH 256
#define GY906_TASK_PRIORITY 25
TaskHandle_t gy906_task_handle = NULL;

/*接收数据 task*/
#define REVDATA_TASK_DEPTH 256
#define REVDATA_TASK_PRIORITY 24
TaskHandle_t revData_task_handle = NULL;


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
	/*初始化硬件*/
	debug_usart1_Init();
	wifi_usart2_Init();
	Airquality_usart4_init();
	pm25_usart6_init();
	lm2904_init();
	motor_init();
	sg90_init();
	aht20_init();
	OLED_Init();
    led_init();
	relay_init();
	buzzer_init();
	atomizer_init();
	
	

	
	OLED_Clear();
	/*OLED显示初始化*/
	/*硬件初始化完成*/
	for(int i = 0;i <= 6;i++)
	{
		OLED_ShowCHinese(i*16 + 1,0,i,1);
	}
	OLED_ShowCHinese(0,0,11,1);
	OLED_ShowCHinese(16,0,12,1);
	OLED_ShowCHinese(32,0,13,1);
	OLED_ShowCHinese(48,0,14,1);
	OLED_ShowCHinese(64,0,15,1);
	OLED_ShowCHinese(80,0,16,1);
	OLED_ShowCHinese(96,0,19,1);
	OLED_ShowCHinese(112,0,20,1);
	

	

    /*创建信号量*/
    /*1. aht20准备发送数据的的信号量*/
	xAht20ReadySemaphore = xSemaphoreCreateBinary();
	
	/*2. air准备发送数据的的信号量*/
	xAirReadySemaphore = xSemaphoreCreateBinary();
 
    /*3. lm2904准备发送数据的的信号量*/
	xLm2904ReadySemaphore = xSemaphoreCreateBinary();

    /*4. gy906准备发送数据的的信号量 */
	xGy906ReadySemaphore = xSemaphoreCreateBinary();
	
    /*5. 准备接收数据的信号量*/
    xRevDataReadySemaphore = xSemaphoreCreateBinary();
	
	/*创建消息队列*/
	/*1. ath20发送数据消息队列*/
	xWeatherDataQueue = xQueueCreate(10,sizeof(weatherData_t));
	
    /*2. air发送数据消息队列 */
    xAirDataQueue = xQueueCreate(10,sizeof(airData_t));

    /*3. lm2904发送数据消息队列*/
    xDbDataQueue = xQueueCreate(10,sizeof(dbData_t));

    /*4. gy906发送数据消息队列*/
    xTemperatureDataQueue = xQueueCreate(5,sizeof(temperatureData_t));

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
		
	/*5. gy906 task*/
	xTaskCreate(
        (TaskFunction_t)gy906_task,
        (char *)"gy906_task",
        (configSTACK_DEPTH_TYPE)GY906_TASK_DEPTH,
        (void *)NULL,
        (UBaseType_t)GY906_TASK_PRIORITY,
        (TaskHandle_t *)gy906_task_handle);

    /*6. revData task*/
//	xTaskCreate(
//        (TaskFunction_t)rev_data_task,
//        (char *)"revData_task",
//        (configSTACK_DEPTH_TYPE)REVDATA_TASK_DEPTH,
//        (void *)NULL,
//        (UBaseType_t)REVDATA_TASK_PRIORITY,
//        (TaskHandle_t *)revData_task_handle);


	vTaskDelay(pdMS_TO_TICKS(500));
//    char task_buffer[256];
//    vTaskList(task_buffer);
//    DEBUG_LOG("任务状态:\n%s\n", task_buffer);
//    DEBUG_LOG("start_task 即将删除\n");
//    DEBUG_LOG("堆剩余: %u\n", xPortGetFreeHeapSize());
    vTaskDelete(NULL); // 删除任务
}

/* 信号量状态监控 */
//void vApplicationIdleHook(void)
//{
//    if(NULL != xWifiReadySemaphore) {
//        UBaseType_t count = uxSemaphoreGetCount(xWifiReadySemaphore);

//        DEBUG_LOG("信号量状态：%ld\n",count);

//    }
//}

void vApplicationMallocFailedHook(void)
{
    DEBUG_LOG("内存分配失败\n");
    taskDISABLE_INTERRUPTS();
    while (1);
}

void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName)
{
    DEBUG_LOG("任务 %s 堆栈溢出\n", pcTaskName);
    taskDISABLE_INTERRUPTS();
    while (1);
}
