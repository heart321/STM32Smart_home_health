#include "bsp_led_driver.h"
#include "stm32f2xx.h"                 

void led_init(void)
{
    // Enable GPIOB clock with stability delay
    __HAL_RCC_GPIOB_CLK_ENABLE();
    
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    
    GPIO_InitStruct.Pin = GPIO_PIN_0;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    
}

void led_toggle(void)
{
	HAL_GPIO_TogglePin(GPIOB,GPIO_PIN_0);
}
