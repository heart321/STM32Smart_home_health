/*
 * @file      bsp_lm2904_driver.c
 *
 * @brief     测量环境中的噪声db
 *
 * @version   V1.0
 *
 * @date     2025/2/6 星期四
 *
 * @auther   chenxuan_123
 *
 * @tips     1tab = 4spaces
 */
#include <math.h>
#include <stdio.h>
#include "bsp_lm2904_driver.h"


//adc dma 句柄
ADC_HandleTypeDef hadc1;
DMA_HandleTypeDef hdma_adc1;

/**
 * @brief  	lm2904初始化adc
 * @param   None
 * @retval  None
 */
void lm2904_init(void)
{
	// 1.开启时钟 ADC1 GPIOA7 DMA2-0
	__HAL_RCC_ADC1_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_DMA2_CLK_ENABLE();
	
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	ADC_ChannelConfTypeDef sConfig = {0};
	
	// 2.配置GPIO
	
	GPIO_InitStruct.Pin = GPIO_PIN_7;
	GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOA,&GPIO_InitStruct);
	
	
	// 3.配置ADC
	hadc1.Instance = ADC1;
	hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV2;
	hadc1.Init.Resolution = ADC_RESOLUTION_12B;
	hadc1.Init.ScanConvMode = ENABLE;
	hadc1.Init.ContinuousConvMode = ENABLE;
	hadc1.Init.DiscontinuousConvMode = DISABLE;
	hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
	hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
	hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
	hadc1.Init.NbrOfConversion = 1;
	hadc1.Init.DMAContinuousRequests = ENABLE;
	hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
	
	if(HAL_ADC_Init(&hadc1) != HAL_OK) while(1);
	
	// 3.1配置ADC通道
	sConfig.Channel = ADC_CHANNEL_7;
	sConfig.Rank = 1;
	sConfig.SamplingTime = ADC_SAMPLETIME_144CYCLES;
	if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK) while(1);
	
	// 4.配置DMA
	hdma_adc1.Instance = DMA2_Stream0;
    hdma_adc1.Init.Channel = DMA_CHANNEL_0;
    hdma_adc1.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_adc1.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_adc1.Init.MemInc = DMA_MINC_ENABLE;
    hdma_adc1.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
    hdma_adc1.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
    hdma_adc1.Init.Mode = DMA_CIRCULAR;
    hdma_adc1.Init.Priority = DMA_PRIORITY_LOW;
    hdma_adc1.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    if (HAL_DMA_Init(&hdma_adc1) != HAL_OK) while(1);

	// 4.1将adc1连接到DMA
	__HAL_LINKDMA(&hadc1, DMA_Handle, hdma_adc1);
	
	/*DMA2 数据流0中断*/
	HAL_NVIC_SetPriority(DMA2_Stream0_IRQn,0,0);
	HAL_NVIC_EnableIRQ(DMA2_Stream0_IRQn);

}

/**
 * @brief  	开始采集adc值
 * @param   None
 * @retval  None
 */
void lm2904_start(uint16_t* data,uint16_t len)
{
	
	HAL_ADC_Start_DMA(&hadc1,(uint32_t*)data,(uint32_t)len);

}

/**
 * @brief  	停止采集adc值
 * @param   None
 * @retval  None
 */
void lm2904_stop(void)
{
	
	HAL_ADC_Stop_DMA(&hadc1);

}


/**
 * @brief  	读取ADC值并转换为分贝
 * @param   None
 * @retval  None
 */
float adc_to_db(uint32_t adc_value) 
{
    // 1. 将ADC值转换为电压
    float voltage = (adc_value / 4096.0f) * 3.3f;
	//printf("voltage:%f  \r\n",voltage);
    // 2. 计算分贝值
    //float db_value = 20 * log10(voltage / 5.0f);
	//float db_value = 1.2345 *((float)adc_value / 4096.0f)*50.0f;
	float db_value = (voltage / 3.3f) * 120.0f;

    return db_value;
}

