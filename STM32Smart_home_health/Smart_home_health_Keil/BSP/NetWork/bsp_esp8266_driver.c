/*
 * @file      bsp_esp8266_driver.c
 *
 * @brief     esp8266驱动
 *
 * @version   V1.0
 *
 * @date     2025/1/18 星期六
 *
 * @auther   chenxuan_123
 *
 * @tips     1tab = 4spaces
 */

/****************System_Include***********************/
#include <string.h>
#include "FreeRTOS.h"
#include "task.h"

/****************BSP_Include***********************/
#include "bsp_esp8266_driver.h"
#include "bsp_usart_driver.h"

unsigned char esp8266_rx_buffer[ESP8266_RX_BUFFER_SIZE];
/*用于判断串口接收是否完成*/
unsigned short esp8266_cnt = 0, esp8266_cntPre = 0;

/*
 * @brief   清空ESP8266接收缓冲区
 * @param   None
 * @retval  None
 */
void ESP8266_Clear(void)
{
    memset(esp8266_rx_buffer, 0, sizeof(esp8266_rx_buffer));
    esp8266_cnt = 0;
}

/*
 * @brief   等待接收数据
 * @param   None
 * @retval  ESP8266状态
 */
ESP8266_StatusTypeDef_t ESP8266_WaitReceive(void)
{
    uint32_t timeout = HAL_GetTick() + (uint32_t)500;
    while (esp8266_cnt == esp8266_cntPre)
    {
        if (HAL_GetTick() > timeout)
        {
            return ESP8266_TIMEOUT;
        }
        //HAL_Delay(10);
    }
    esp8266_cntPre = esp8266_cnt;
    return ESP8266_OK;
}

/*
 * @brief   发送AT指令
 * @param   cmd AT指令
 * 			res 判断返回值
 *			waittime 发送事件
 * @retval  ESP8266状态
 */
ESP8266_StatusTypeDef_t ESP8266_SendCmd(char *cmd, char *res, uint32_t waittime)
{
    ESP8266_Clear();
    wifi_usart2_Send((uint8_t *)cmd, strlen(cmd));
    wifi_usart2_Receive(&esp8266_rx_buffer[esp8266_cnt], 1);
    while (waittime--)
    {
        if (ESP8266_WaitReceive() == ESP8266_OK)
        {
            if (strstr((char *)esp8266_rx_buffer, res) != NULL)
            {
                ESP8266_Clear();
                return ESP8266_OK;
            }
        }
        // vTaskDelay(pdMS_TO_TICKS(10));
    }
    return ESP8266_TIMEOUT;
}

/*
 * @brief   连接wifi网络
 * @param   None
 * @retval  ESP8266状态
 */

ESP8266_StatusTypeDef_t ESP8266_Connect(void)
{
    ESP8266_Clear();

    // 发送AT指令，等待返回OK，如果返回错误，则发送AT failed，并返回ESP8266_ERROR
#if DEBUG
    DEBUG_LOG("AT\n");
#endif
    while (ESP8266_SendCmd("AT\r\n", "OK", 200) != ESP8266_OK)
    {
#if DEBUG
        DEBUG_LOG("AT ERROR!\n");
        DEBUG_LOG("rx_buffer:%s \n", esp8266_rx_buffer);
#endif
        HAL_Delay(200);
        return ESP8266_ERROR;
    }
#if DEBUG
    DEBUG_LOG("AT OK\n");
#endif
    // 发送AT+CWMODE=1指令，等待返回OK，如果返回错误，则发送AT+CWMODE=1 failed，并返回ESP8266_ERROR
#if DEBUG
    DEBUG_LOG("AT+CWMODE=1\n");
#endif
    while (ESP8266_SendCmd("AT+CWMODE=1\r\n", "OK", 200) != ESP8266_OK)
    {
#if DEBUG
        DEBUG_LOG("AT+CWMODE=1 ERROR\n");
        DEBUG_LOG("rx_buffer:%s \n", esp8266_rx_buffer);
#endif
        HAL_Delay(200);
        return ESP8266_ERROR;
    }

    // 发送ESP8266_WIFI指令，等待返回OK，如果返回错误，则发送AT+CWJAP failed，并返回ESP8266_ERROR
#if DEBUG
    DEBUG_LOG("ESP8266_WIFI\n");
#endif
    while (ESP8266_SendCmd(ESP8266_WIFI, "OK", 200) != ESP8266_OK)
    {
#if DEBUG

        DEBUG_LOG("ESP8266_WIFI ERROR\n");
        DEBUG_LOG("rx_buffer:%s \n", esp8266_rx_buffer);
#endif
        HAL_Delay(200);
        return ESP8266_ERROR;
    }

    // 发送ESP8266_TCP指令，等待返回OK，如果返回错误，则发送AT+CIPSTART failed，并返回ESP8266_ERROR
#if DEBUG
    DEBUG_LOG("ESP8266_TCP\n");
#endif
    while (ESP8266_SendCmd(ESP8266_TCP, "OK", 200) != ESP8266_OK)
    {
#if DEBUG
        DEBUG_LOG("ESP8266_TCP ERROR\n");
#endif
        HAL_Delay(200);
        return ESP8266_ERROR;
    }

    // Debug_usart3_Send((uint8_t*)esp8266_rx_buffer,3);
    //  发送ESP8266 Init OK
#if DEBUG
    DEBUG_LOG("ESP8266 Init OK\n");
#endif

    // 返回ESP8266_OK
    return ESP8266_OK;
}

/*
 * @brief   ESP8266发送数据
 * @param   *data 数据地址
 * 			len 数据长度
 * @retval  ESP8266状态
 */
ESP8266_StatusTypeDef_t ESP8266_SendData(char *data, uint16_t len)
{
    char cmdBUF[32];
    ESP8266_Clear();

    sprintf(cmdBUF, "AT+CIPSEND=%d\r\n", len);
    if (ESP8266_SendCmd(cmdBUF, ">", 200) == ESP8266_OK)
    {
        wifi_usart2_Send((uint8_t *)data, len);
        return ESP8266_OK;
    }
    return ESP8266_ERROR;
}

/*
 * @brief   从ESP8266接收TCP数据
 *          解析 "+IPD,x:yyy" 格式，其中 x 是数据长度，yyy 是数据内容
 * @param   waittime_ms 等待时间（毫秒）
 * @retval  指向数据的指针，若失败返回 NULL
 */
unsigned char *ESP8266_GetIPD(unsigned short waittime_ms)
{
    char *ptrIPD = NULL;

    // 清空缓冲区，确保从头开始接收
    ESP8266_Clear();
    do
    {
        if (ESP8266_WaitReceive() == ESP8266_OK)
        {
            // 打印当前缓冲区内容，便于调试
            // DEBUG_LOG("rx_data: %s\n", esp8266_rx_buffer);

            // 查找 "+IPD," 前缀
            ptrIPD = strstr((char *)esp8266_rx_buffer, "IPD,");
            if (ptrIPD == NULL)
            {
                // DEBUG_LOG("No IPD found in buffer\n");
            }
            else
            {
                // 查找数据起始位置
                ptrIPD = strchr(ptrIPD, ':');
                if (ptrIPD != NULL)
                {
                    ptrIPD++; // 跳过 ':'
#if DEBUG
                    DEBUG_LOG("IPD data found: %s\n", ptrIPD);
#endif
                    return (unsigned char *)ptrIPD;
                }
                else
                {
                    // DEBUG_LOG("Invalid IPD format, missing ':'\n");
                    return NULL;
                }
            }
        }
        HAL_Delay(10);
        waittime_ms--;
    } while (waittime_ms > 0);
    return NULL;
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (USART2 == huart->Instance)
    {
        UBaseType_t status_value;
        status_value = taskENTER_CRITICAL_FROM_ISR();
        if (esp8266_cnt < ESP8266_RX_BUFFER_SIZE - 1)
        {
            esp8266_rx_buffer[esp8266_cnt++] = huart->Instance->DR;
        }
        taskEXIT_CRITICAL_FROM_ISR(status_value);
        wifi_usart2_Receive(&esp8266_rx_buffer[esp8266_cnt], 1);
    }
}
