#ifndef __BSP_AIRQUALITY_DRIVER_H__
#define __BSP_AIRQUALITY_DRIVER_H__

#include "stm32f2xx.h"

void Airquality_usart4_init(void);

void Airquality_send_data(uint8_t* data,uint16_t len);

void Airquality_rev_data(uint8_t* data,uint16_t len);


#endif
