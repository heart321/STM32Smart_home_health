/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f2xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define Relay_1_Pin GPIO_PIN_0
#define Relay_1_GPIO_Port GPIOC
#define Relay_2_Pin GPIO_PIN_1
#define Relay_2_GPIO_Port GPIOC
#define Buzzer_Pin GPIO_PIN_2
#define Buzzer_GPIO_Port GPIOC
#define WUHUA_Pin GPIO_PIN_4
#define WUHUA_GPIO_Port GPIOA
#define LED0_Pin GPIO_PIN_5
#define LED0_GPIO_Port GPIOA
#define LED1_Pin GPIO_PIN_6
#define LED1_GPIO_Port GPIOA
#define MAX30102_INT_PIN_Pin GPIO_PIN_1
#define MAX30102_INT_PIN_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

typedef enum
{
	TEMP_AND_HUMI,
	AIR,
	DB,
	PEOPLE_TEMP,
	MAX30102
}SensorType_t;
	
/*传感器数据结构体*/
typedef struct
{
    SensorType_t type;
    union
    {
        struct { // SENSOR_TEMP_AND_HUMI
            float temperature;
            float humidity;
        } aht20;
        struct { // SENSOR_DB
            uint32_t adc_DB;
            float DB;
        } db;
        struct { // SENSOR_PEOPLE_TEMP
            float people_temp;
        } gy906;
        struct { // SENSOR_MAX30102
            int heart_rate;
            int spo2;
        } max30102;
        struct { // SENSOR_AIR
            float TVOC;
            int CO2;
            float HCHO;
            int PM25;
        } air;
    }sensor_values;
} SensorData_t;

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
