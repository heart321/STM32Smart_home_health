#include "bsp_drv8833_driver.h"
#include "tim.h"

/*
 * @brief 设置drv8833 电机方向
 * @param   direction = forword 正转
 *          direction = reversal 反转
 * @retval None
*/
void drv8833_motor_direction(motor_direction_t direction)
{
    if(direction == forword)
    {
        HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_4);
        HAL_GPIO_WritePin(GPIOB,GPIO_PIN_9,GPIO_PIN_RESET);
    }
    else if(direction == reversal)
    {
        HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_3);
        HAL_GPIO_WritePin(GPIOB,GPIO_PIN_8,GPIO_PIN_RESET);
    }
}


/*
 * @brief 设置电机速度
 * @param speed 速度值
 * @retval None
*/
void drv8833_motor_speed(uint32_t speed)
{
    __HAL_TIM_SET_COMPARE(&htim4,TIM_CHANNEL_4,speed);
	__HAL_TIM_SET_COMPARE(&htim4,TIM_CHANNEL_3,speed);
}

/*
 * @brief 电机停止
 * @param None
 * @retval None
*/
void drv8833_motot_stop(void)
{
    HAL_TIM_PWM_Stop(&htim4, TIM_CHANNEL_4);
	HAL_TIM_PWM_Stop(&htim4, TIM_CHANNEL_3);
}

