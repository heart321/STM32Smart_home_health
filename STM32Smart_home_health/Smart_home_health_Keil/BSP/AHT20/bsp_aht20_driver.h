#ifndef __BSP_AHT20_DRIVER_H__
#define __BSP_AHT20_DRIVER_H__

#include "stm32f2xx_hal.h"

void aht20_init(void);


void aht20_read(float* temp,float* humi);


#endif 
