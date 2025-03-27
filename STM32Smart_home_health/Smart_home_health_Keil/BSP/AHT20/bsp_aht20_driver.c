/*
 * @file      bsp_aht20_driver.c
 *
 * @brief     AHT20温湿度传感器驱动
 *
 * @version   V1.0
 *
 * @date     2025/1/27 星期一
 *
 * @auther   chenxuan_123
 */

#include "bsp_aht20_driver.h"
#include "i2c.h"


#define AHT20_ADDRESS 0x70



/*
 * @brief   AHT20初始化
 * @param   None
 * @retval  None
 */
void aht20_init(void)
{


    // 测试AHT20是否响应
    HAL_Delay(100); // 上电等待
    if (HAL_I2C_IsDeviceReady(&hi2c1, AHT20_ADDRESS, 1, HAL_MAX_DELAY) != HAL_OK) {
        while (1);
    }

    // 初始化AHT20
    uint8_t readBuffer;
    HAL_I2C_Master_Receive(&hi2c1, AHT20_ADDRESS, &readBuffer, 1, HAL_MAX_DELAY);

    if ((readBuffer & 0x08) == 0x00) {
        uint8_t sendBuffer[3] = {0xBE, 0x08, 0x00};
        HAL_I2C_Master_Transmit(&hi2c1, AHT20_ADDRESS, sendBuffer, 3, HAL_MAX_DELAY);
    }
}


/*
 * @brief   读取AHT20温湿度数据
 * @param   temp 温度数据 humi 湿度数据
 * @retval  None
 */

void aht20_read(float* temp, float* humi) {
    uint8_t sendBuffer[3] = {0xAC, 0x33, 0x00};
    uint8_t readBuffer[6];
    uint8_t status;

    // 发送测量命令
    HAL_I2C_Master_Transmit(&hi2c1, AHT20_ADDRESS, sendBuffer, 3, HAL_MAX_DELAY);

    // 等待测量完成
    uint32_t timeout = 100; // 超时100ms
    do {
        HAL_I2C_Master_Receive(&hi2c1, AHT20_ADDRESS, &status, 1, HAL_MAX_DELAY);
        HAL_Delay(10);
        timeout -= 10;
    } while ((status & 0x80) && timeout > 0);

    if (timeout == 0) {
        *temp = 0;
        *humi = 0;
        return;
    }

    // 读取数据
    HAL_I2C_Master_Receive(&hi2c1, AHT20_ADDRESS, readBuffer, 6, HAL_MAX_DELAY);

    // 解析数据
    uint32_t humi_data = ((uint32_t)readBuffer[1] << 12) | 
                         ((uint32_t)readBuffer[2] << 4) | 
                         ((readBuffer[3] >> 4) & 0x0F);
    *humi = (humi_data * 100.0f) / (1 << 20);

    uint32_t temp_data = ((uint32_t)(readBuffer[3] & 0x0F) << 16) | 
                         ((uint32_t)readBuffer[4] << 8) | 
                         readBuffer[5];
    *temp = (temp_data * 200.0f) / (1 << 20) - 50;
}



