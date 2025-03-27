#include "bsp_buzzer_driver.h"

void buzzer_on(void)
{
    HAL_GPIO_WritePin(GPIOC,GPIO_PIN_2,GPIO_PIN_SET);
}
void buzzer_off(void)
{
    HAL_GPIO_WritePin(GPIOC,GPIO_PIN_2,GPIO_PIN_RESET);
}





