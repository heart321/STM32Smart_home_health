#ifndef __BSP_GY906_DRIVER_H__
#define __BSP_GY906_DRIVER_H__

#include "stm32f2xx.h"

float mlx90614_readTemp(uint8_t reg);

float readObjectTemp(void);

float readAmbientTemp(void);

#endif
