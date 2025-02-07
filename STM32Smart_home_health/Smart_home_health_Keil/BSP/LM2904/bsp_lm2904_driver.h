#ifndef __BSP_LM2904_DRIVER_H__
#define __BSP_LM2904_DRIVER_H__

#include "stm32f2xx.h"

void lm2904_init(void);

void lm2904_start(uint16_t* data,uint16_t len);
void lm2904_stop(void);
float adc_to_db(uint32_t adc_value) ;


#endif 
