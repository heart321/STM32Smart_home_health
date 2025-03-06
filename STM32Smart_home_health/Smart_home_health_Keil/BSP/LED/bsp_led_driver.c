#include "bsp_led_driver.h"
#include "stm32f2xx.h"                 

void led_init(void)
{
    // Enable GPIOB clock with stability delay
	if(__HAL_RCC_GPIOA_IS_CLK_DISABLED())
	{
		__HAL_RCC_GPIOA_CLK_ENABLE();
	}
    
    
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    
    GPIO_InitStruct.Pin = GPIO_PIN_5 | GPIO_PIN_6;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    HAL_GPIO_WritePin(GPIOA,GPIO_PIN_5,GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOA,GPIO_PIN_6,GPIO_PIN_SET);
    
}

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
