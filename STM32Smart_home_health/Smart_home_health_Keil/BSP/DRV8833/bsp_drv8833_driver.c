#include "bsp_drv8833_driver.h"

// 预分频器4
#define TIM4_PRESCALER  (5 - 1) 
// 自动重装值 99
#define TIM4_PERIOD     (100 - 1)

// 定时器4句柄
TIM_HandleTypeDef htim4;


/*
 * @brief drv8833 电机初始化
 * @param None
 * @retval None
*/
void drv8833_motor_init(void)
{
    //1.开启时钟
	if(__HAL_RCC_GPIOB_IS_CLK_DISABLED())
	{
		__HAL_RCC_GPIOB_CLK_ENABLE();
	}
	if(__HAL_RCC_TIM4_IS_CLK_DISABLED())
	{
		__HAL_RCC_TIM4_CLK_ENABLE();
	}
	
	
	
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	TIM_ClockConfigTypeDef sClockSourceConfig = {0};
	TIM_MasterConfigTypeDef sMasterConfig = {0};
	TIM_OC_InitTypeDef sConfigOC = {0};
	
	//2.配置GPIO
    GPIO_InitStruct.Pin = GPIO_PIN_8|GPIO_PIN_9;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF2_TIM4;
	HAL_GPIO_Init(GPIOB,&GPIO_InitStruct);
	
	//3.配置定时器
	htim4.Instance = TIM4;
	htim4.Init.Prescaler = TIM4_PRESCALER;
	htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim4.Init.Period = TIM4_PERIOD;
	htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htim4.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
	
	HAL_TIM_Base_Init(&htim4);
	
	// 配置定时器时钟源
	sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
	HAL_TIM_ConfigClockSource(&htim4,&sClockSourceConfig);
	
	// 初始化PWM模式
	HAL_TIM_PWM_Init(&htim4);
	
	sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	HAL_TIMEx_MasterConfigSynchronization(&htim4, &sMasterConfig);

	sConfigOC.OCMode = TIM_OCMODE_PWM1;
	sConfigOC.Pulse = 0;
	sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
	sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
	
	HAL_TIM_PWM_ConfigChannel(&htim4, &sConfigOC, TIM_CHANNEL_3);
	HAL_TIM_PWM_ConfigChannel(&htim4, &sConfigOC, TIM_CHANNEL_4);
	
	
	
}

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

