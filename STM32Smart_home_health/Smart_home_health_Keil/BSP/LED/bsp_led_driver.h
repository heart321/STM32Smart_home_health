#ifndef __BSP_LED_DRIVER_H__
#define __BSP_LED_DRIVER_H__

#include "stm32f2xx_hal.h"

typedef struct
{
	GPIO_TypeDef *port;
	uint32_t pin;
}Led_t;

void led_on(Led_t led);

void led_off(Led_t led);


void led_toggle(Led_t led);




#endif /* __BSP_LED_DRIVER_H__ */
