/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * File Name          : freertos.c
 * Description        : Code for freertos applications
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2025 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "semphr.h"
#include "queue.h"
#include "list.h"
/****************Task_Include***********************/
#include "app_mqttSend_task.h"
#include "app_aht20_task.h"
#include "app_air_task.h"
#include "app_lm2904_task.h"
#include "app_gy906_task.h"
#include "app_mqttRev_task.h"
#include "app_max30102_task.h"
/****************BSP_Include***********************/
#include "bsp_usart_driver.h"
#include "bsp_aht20_driver.h"
#include "bsp_lm2904_driver.h"
#include "bsp_airquality_driver.h"
#include "bsp_oled_driver.h"
#include "bsp_led_driver.h"
#include "bsp_pm2.5_driver.h"
#include "bsp_sg90_driver.h"
#include "bsp_led_driver.h"
#include "bsp_relay_driver.h"
#include "bsp_buzzer_driver.h"
#include "bsp_wuhua_driver.h"
#include "bsp_esp8266_driver.h"
#include "bsp_drv8833_driver.h"
#include "bsp_max30102_driver.h" 
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/****************Task_Init**************************/
/*start_task*/
#define START_TASK_DEPTH 256
#define START_TASK_PRIORITY 1
TaskHandle_t start_task_handle = NULL;

/*mqtt_send_task*/
#define MQTT_SEND_TASK_DEPTH 1024
#define MQTT_SEND_TASK_PRIORITY 27
TaskHandle_t mqtt_send_handle = NULL;

/*aht20_task*/
#define AHT20_TASK_DEPTH 256
#define AHT20_TASK_PRIORITY 24
TaskHandle_t aht20_task_handle = NULL;

/*air_task*/
#define AIR_TASK_DEPTH 256
#define AIR_TASK_PRIORITY 24
TaskHandle_t air_task_handle = NULL;

/*lm2904 噪声 task*/
#define LM2904_TASK_DEPTH 256
#define LM2904_TASK_PRIORITY 24
TaskHandle_t lm2904_task_handle = NULL;

/*gy906 体温 task*/
#define GY906_TASK_DEPTH 256
#define GY906_TASK_PRIORITY 24
TaskHandle_t gy906_task_handle = NULL;

/*接收数据 task*/
#define MQTT_REV_TASK_DEPTH 512
#define MQTT_REV_TASK_PRIORITY 24
TaskHandle_t mqtt_rev_task_handle = NULL;

/*max30102 task*/
#define MAX30102_TASK_DEPTH 512
#define MAX30102_TASK_PRIORITY 26
TaskHandle_t max30102_task_handle = NULL;

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
SemaphoreHandle_t xAirQualitySemaphore; // 空气质量数据信号Semaphore
SemaphoreHandle_t xPM25Semaphore;       // PM2.5数据Semaphore
/*传感器发送数据队Queue*/
QueueHandle_t xSensorDataQueue = NULL;
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

QueueHandle_t xMedQueue;  // 医疗数据队列
/* USER CODE END Variables */
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* Hook prototypes */
void vApplicationStackOverflowHook(xTaskHandle xTask, signed char *pcTaskName);

/* USER CODE BEGIN 4 */
void vApplicationStackOverflowHook(xTaskHandle xTask, signed char *pcTaskName)
{
  /* Run time stack overflow checking is performed if
  configCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2. This hook function is
  called if a stack overflow is detected. */
	  printf("Stack overflow in task: %s\n", pcTaskName);
		while (1); // 保留死循环以便调�??
}
/* USER CODE END 4 */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */

  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */

  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */

  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
 * @brief  Function implementing the defaultTask thread.
 * @param  argument: Not used
 * @retval None
 */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* USER CODE BEGIN StartDefaultTask */

  /* Infinite loop */
  /*创建消息队列 */
  xSensorDataQueue = xQueueCreate(20, sizeof(SensorData_t));
  /*创建信号*/
  xAirQualitySemaphore = xSemaphoreCreateBinary();
  xPM25Semaphore = xSemaphoreCreateBinary();
  /*创建其他任务*/
  /*1. mqttsend*/
  xTaskCreate(
      (TaskFunction_t)mqtt_send_task,
      (char *)"mqtt_send_connect",
      (configSTACK_DEPTH_TYPE)MQTT_SEND_TASK_DEPTH,
      (void *)NULL,
      (UBaseType_t)MQTT_SEND_TASK_PRIORITY,
      (TaskHandle_t *)&mqtt_send_handle);

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

  /*4. lm2904 task*/
  xTaskCreate(
      (TaskFunction_t)lm2904_task,
      (char *)"lm2904_task",
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
  xTaskCreate(
      (TaskFunction_t)mqtt_rev_task,
      (char *)"revData_task",
      (configSTACK_DEPTH_TYPE)MQTT_REV_TASK_DEPTH,
      (void *)NULL,
      (UBaseType_t)MQTT_REV_TASK_PRIORITY,
      (TaskHandle_t *)mqtt_rev_task_handle);

  /*7. max30102 task*/
  xTaskCreate(
      (TaskFunction_t)max30102_task,
      (char *)"max30102_task",
      (configSTACK_DEPTH_TYPE)MAX30102_TASK_DEPTH,
      (void *)NULL,
      (UBaseType_t)MAX30102_TASK_PRIORITY,
      (TaskHandle_t *)max30102_task_handle);

  vTaskDelay(pdMS_TO_TICKS(100));

  vTaskDelete(NULL); // 删除任务
}
  /* USER CODE END StartDefaultTask */


/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

