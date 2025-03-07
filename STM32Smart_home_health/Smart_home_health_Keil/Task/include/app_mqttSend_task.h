#ifndef __APP_WIFI_TASK_H__
#define __APP_WIFI_TASK_H__
#include "stm32f2xx_hal.h"

void mqtt_send_task(void *pvParameters);
HAL_StatusTypeDef wifi_mqtt_init(void);


#endif
