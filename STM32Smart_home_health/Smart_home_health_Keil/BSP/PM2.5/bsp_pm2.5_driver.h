#ifndef __BSP_PM25_DRIVER_H__
#define __BSP_PM25_DRIVER_H__

#include "stm32f2xx.h"


void pm25_usart6_init(void);
void pm25_rev_data(uint8_t* data,uint16_t len);

#endif
