#include "bsp_sg90_driver.h"


TIM_HandleTypeDef htim1;



void sg90_init(void)
{
	
	if(__HAL_RCC_GPIOA_IS_CLK_DISABLED())
	{
		__HAL_RCC_GPIOA_CLK_ENABLE();  
	}
	if(__HAL_RCC_TIM1_IS_CLK_DISABLED())
	{
		__HAL_RCC_TIM1_CLK_ENABLE();
	}
	
	
	GPIO_InitTypeDef GPIO_InitStruct = {0};
    
    GPIO_InitStruct.Pin = GPIO_PIN_11;           // TIM1_CH4通常在PA11
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;      // 复用推挽输出
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF1_TIM1;   // TIM1的复用功能
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	
	
	TIM_OC_InitTypeDef sConfigOC = {0};
    
    htim1.Instance = TIM1;
    htim1.Init.Prescaler = 120 - 1;           // 分频系数：120MHz/120=1MHz
    htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim1.Init.Period = 20000 - 1;            // PWM周期：1MHz/20000=50Hz
    htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim1.Init.RepetitionCounter = 0;
    HAL_TIM_PWM_Init(&htim1);
    
    // 配置TIM1通道4为PWM模式
    sConfigOC.OCMode = TIM_OCMODE_PWM1;
    sConfigOC.Pulse = 1500;                   // 初始脉宽1.5ms (90°)
    sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
    sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
    sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
    sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
    HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_4);
	
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_4);
	
}

void sg90_set_angle(uint8_t angle)
{
	// SG90脉宽范围：0.5ms-2.5ms对应0-180度
    // 计算公式：Pulse = 500 + (angle * 2000) / 180
    uint32_t pulse = 500 + ((uint32_t)angle * 2000) / 180;
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_4, pulse);

}








