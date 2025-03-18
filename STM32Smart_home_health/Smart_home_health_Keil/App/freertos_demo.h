#ifndef __FREERTOS_DEMO_H__
#define __FREERTOS_DEMO_H__

#include "stm32f2xx_hal.h"

/*传感器类型*/
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
	SensorType_t type; //传感器类型
	float temperature;
    float humidity;
	float TVOC;
    float CO2;
    float HCHO;
    int   PM25;
	uint16_t adc_value;
    float db_value;
	float people_temp;
	uint8_t dis_hr;     //心率
    uint8_t dis_spo2;   //血氧
}SensorData_t;

void freeRTOS_start(void);


#endif /* __FREERTOS_DEMO_H__ */
