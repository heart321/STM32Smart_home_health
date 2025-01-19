#ifndef __BSP_LED_DRIVER_H__
#define __BSP_LED_DRIVER_H__

#include "stm32f2xx_hal.h"

/**
  * @brief  Initialize LED GPIO
  * @retval None
  */
void led_init(void);

/**
  * @brief  Toggle LED state
  * @retval None
  */
void led_toggle(void);

#endif /* __BSP_LED_DRIVER_H__ */
