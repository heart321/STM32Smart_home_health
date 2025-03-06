#ifndef __BSP_RELAY_H__
#define __BSP_RELAY_H__

#include "stm32f2xx_hal.h"

typedef struct
{
    GPIO_TypeDef *port;
	uint32_t pin;
}Relay_t;

void relay_init(void);

void relay_on(Relay_t relay);

void relay_off(Relay_t relay);


#endif 
