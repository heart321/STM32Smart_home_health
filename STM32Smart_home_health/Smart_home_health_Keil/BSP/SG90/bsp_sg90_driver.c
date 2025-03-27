#include "bsp_sg90_driver.h"
#include "tim.h"

void sg90_init(void)
{
		HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_4);
}


void sg90_set_angle(uint8_t angle)
{
	// SG90脉宽范围：0.5ms-2.5ms对应0-180度
    // 计算公式：Pulse = 500 + (angle * 2000) / 180
    uint32_t pulse = 500 + ((uint32_t)angle * 2000) / 180;
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_4, pulse);

}








