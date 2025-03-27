#include "bsp_relay_driver.h"


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
