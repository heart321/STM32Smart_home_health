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
#include "adc.h"


/**
 * @brief  	开始采集adc值
 * @param   None
 * @retval  None
 */
void lm2904_start(uint32_t* data,uint32_t len)
{
	
	HAL_ADC_Start_DMA(&hadc1,data,len);

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

