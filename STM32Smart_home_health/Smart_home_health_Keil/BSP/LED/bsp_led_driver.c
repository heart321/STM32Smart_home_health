#include "bsp_led_driver.h"
#include "stm32f2xx.h"                 


void led_on(Led_t led)
{
	HAL_GPIO_WritePin(led.port,led.pin,GPIO_PIN_RESET);
}

void led_off(Led_t led)
{
	HAL_GPIO_WritePin(led.port,led.pin,GPIO_PIN_SET);
}

void led_toggle(Led_t led)
{
	HAL_GPIO_TogglePin(led.port,led.pin);
}
