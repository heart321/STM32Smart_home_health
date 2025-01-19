#include "stm32f2xx.h"
#include "sys.h"
#include "freertos_demo.h"

#include "bsp_led_driver.h"
#include "bsp_usart_driver.h"
#include "bsp_esp8266_driver.h"


int main(void)
{
    // Initialize HAL Library
    HAL_Init();
    // Configure the system clock  120Mhz
    sys_clock_init();
    // Satrt FreeRTOS
	freeRTOS_start();
	
    while(1)
    {

    }
}
