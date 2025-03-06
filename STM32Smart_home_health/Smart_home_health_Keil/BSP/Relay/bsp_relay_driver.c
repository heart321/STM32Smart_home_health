#include "bsp_relay_driver.h"

void relay_init(void)
{
    if(__HAL_RCC_GPIOC_IS_CLK_DISABLED())
    {
        __HAL_RCC_GPIOC_CLK_ENABLE();
    }

    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = GPIO_PIN_1 | GPIO_PIN_0;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    HAL_GPIO_WritePin(GPIOC,GPIO_PIN_0,GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOC,GPIO_PIN_1,GPIO_PIN_RESET);
}

void relay_on(Relay_t relay)
{
    HAL_GPIO_WritePin(relay.port,relay.pin,GPIO_PIN_SET);
}

void relay_off(Relay_t relay)
{
    HAL_GPIO_WritePin(relay.port,relay.pin,GPIO_PIN_RESET);
}

int relat_read(Relay_t relay)
{
    if(HAL_GPIO_ReadPin(relay.port,relay.pin) == GPIO_PIN_SET)
    {
        return 1;
    }else return 0;
}
