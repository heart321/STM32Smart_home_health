/*
 * @file      sys.c
 *
 * @brief      系统时钟初始化
 *
 * @version   V1.0
 *
 * @date     2025/1/16 星期四
 *
 * @auther   chenxuan_123
 */


#include "sys.h"
#include "stm32f2xx.h"                  // Device header
#include "stm32f2xx_hal_rcc.h"
#include "stm32f2xx_hal_rcc_ex.h"
#include "stm32f2xx_hal_tim.h"


/**
  * @brief System Clock Configuration
  * @retval None
  */
void sys_clock_init(void)
{
    HAL_StatusTypeDef ret = HAL_ERROR;
    RCC_OscInitTypeDef rcc_osc_init;
    RCC_ClkInitTypeDef rcc_clk_init;

    rcc_osc_init.OscillatorType = RCC_OSCILLATORTYPE_HSE;   /*选择要配置HSE*/
    rcc_osc_init.HSEState = RCC_HSE_ON;                     /*使能HSE*/
    rcc_osc_init.PLL.PLLState = RCC_PLL_ON;                 /*使能PLL*/
    rcc_osc_init.PLL.PLLSource = RCC_PLLSOURCE_HSE;         /*选择PLL的时钟源为HSE*/
    rcc_osc_init.PLL.PLLM = 8;                              /*HSE分频系数为8*/
    rcc_osc_init.PLL.PLLN = 240;                            /*PLL倍频系数为240*/
    rcc_osc_init.PLL.PLLP = RCC_PLLP_DIV2;                  /*PLL输出时钟分频系数为2*/
    rcc_osc_init.PLL.PLLQ = 4;                             /*PLL输出时钟分频系数为4*/
    ret = HAL_RCC_OscConfig(&rcc_osc_init);
    if (ret != HAL_OK)
    {
        while (1); //配置失败，死循环
    }
    /*
    RCC_CLOCKTYPE_HCLK : AHB时钟        120MHz
    RCC_CLOCKTYPE_SYSCLK: 系统时钟        120MHz
    RCC_CLOCKTYPE_PCLK1: APB1时钟        30MHz       APB1 Timer = 60MHz
    RCC_CLOCKTYPE_PCLK2: APB2时钟        60MHz       APB2 Timer = 120MHz
    */
    rcc_clk_init.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
                            | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2; 
    rcc_clk_init.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    rcc_clk_init.AHBCLKDivider = RCC_SYSCLK_DIV1;
    rcc_clk_init.APB1CLKDivider = RCC_HCLK_DIV4;
    rcc_clk_init.APB2CLKDivider = RCC_HCLK_DIV2;
    ret = HAL_RCC_ClockConfig(&rcc_clk_init, FLASH_LATENCY_3);
    if (ret != HAL_OK)
    {
        while (1); //配置失败，死循环
    }

}

/**
  * @brief  周期完成回调函数（非阻塞模式）
  * @note   当 TIM4 中断发生时会调用此函数，调用位置在 HAL_TIM_IRQHandler() 内。
  *         它直接调用 HAL_IncTick() 来增加全局变量 "uwTick"，
  *         该变量用作应用程序的时间基准。
  * @param  htim : 定时器句柄
  * @retval 无
  */

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim->Instance == TIM4)
	{
		HAL_IncTick();
	}
}




