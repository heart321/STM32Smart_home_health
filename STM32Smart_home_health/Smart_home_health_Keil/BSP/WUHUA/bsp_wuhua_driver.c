#include "bsp_wuhua_driver.h"

void atomizer_on(void)
{
    HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4,GPIO_PIN_SET);
}

void atomizer_off(void)
{
    HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4,GPIO_PIN_RESET);
}

int atomizer_read(void)
{
	if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_4) == GPIO_PIN_SET)
	{
		return 1;
	}else return 0;
		
}

