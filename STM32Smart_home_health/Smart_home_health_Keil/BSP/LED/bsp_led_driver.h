#ifndef __BSP_LED_DRIVER_H__
#define __BSP_LED_DRIVER_H__

#include "stm32f2xx_hal.h"

typedef struct
{
	GPIO_TypeDef *port;
	uint32_t pin;
}Led_t;

/**
  * @brief  Initialize LED GPIO
  * @retval None
  */
void led_init(void);



#endif /* __BSP_LED_DRIVER_H__ */
