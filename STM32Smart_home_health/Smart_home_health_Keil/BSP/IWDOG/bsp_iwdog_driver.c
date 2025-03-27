#include "bsp_iwdog_driver.h"
#include "iwdg.h"



void iwdog_refresh(void)
{
    HAL_IWDG_Refresh(&hiwdg);
}

