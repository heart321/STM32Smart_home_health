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




/*串口接收缓冲区*/
#define ESP8266_RX_BUFFER_SIZE 256
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
    static uint32_t lastTick = 0;

    if (esp8266_cnt == 0)
    {
        return ESP8266_ERROR;
    }
	
	/*数据不在发生变化 说明已经接收完成*/
    uint32_t currentTick = xTaskGetTickCount();
    if (currentTick - lastTick > 100)
    {
        esp8266_cntPre = esp8266_cnt;
        lastTick = currentTick; // 重置 lastTick
        return ESP8266_OK;
    }
	
    if (esp8266_cnt != esp8266_cntPre)
    {
        lastTick = currentTick; // 更新 lastTick
        esp8266_cntPre = esp8266_cnt;
    }

    return ESP8266_ERROR;
}



/*
 * @brief   发送AT指令
 * @param   cmd AT指令
 * 			res 判断返回值
 *			waittime 发送事件
 * @retval  ESP8266状态
 */
ESP8266_StatusTypeDef_t ESP8266_SendCmd(char *cmd,char *res, uint32_t waittime)
{

    
    ESP8266_Clear();
    
   
    wifi_usart2_Send((uint8_t*)cmd, strlen(cmd));
    
    
    wifi_usart2_Receive(&esp8266_rx_buffer[0], 1);
    
   
    while(waittime--)
    {
        if(ESP8266_WaitReceive() == ESP8266_OK)
        {
            if(strstr((char*)esp8266_rx_buffer, res) != NULL)
            {
                ESP8266_Clear();
                return ESP8266_OK;
            }
        }
        HAL_Delay(10);
    }
    
#if DEBUG
    DEBUG_LOG("Timeout\n");
#endif
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
    while(ESP8266_SendCmd("AT\r\n", "OK", 1000) != ESP8266_OK)
    {
#if DEBUG
        DEBUG_LOG("AT ERROR!\n");
		DEBUG_LOG("rx_buffer:%s \n",esp8266_rx_buffer);
#endif
        HAL_Delay(1000);
        return ESP8266_ERROR;
    }
    
	/*AT+RST发送出去导致连接不上wifi ?? 不清楚原因*/
    // 发送AT+RST指令，等待返回OK，如果返回错误，则发送AT+RST failed，并返回ESP8266_ERROR
//#if DEBUG
//    DEBUG_LOG("AT+RST\n");
//#endif
//    while(ESP8266_SendCmd("AT+RST\r\n", "OK", 1000) != ESP8266_OK)
//    {
//#if DEBUG
//        DEBUG_LOG("AT+RST ERROR\n");
//		Debug_usart3_Send((uint8_t*)esp8266_rx_buffer,128);
//#endif
//        HAL_Delay(1000);
//        return ESP8266_ERROR;
//    }
    
    
    // 发送AT+CWMODE=1指令，等待返回OK，如果返回错误，则发送AT+CWMODE=1 failed，并返回ESP8266_ERROR
#if DEBUG
    DEBUG_LOG("AT+CWMODE=1\n");
#endif
    while(ESP8266_SendCmd("AT+CWMODE=1\r\n", "OK", 1000) != ESP8266_OK)
    {
#if DEBUG
        DEBUG_LOG("AT+CWMODE=1 ERROR\n");
		DEBUG_LOG("rx_buffer:%s \n",esp8266_rx_buffer);
#endif
        HAL_Delay(1000);
        return ESP8266_ERROR;
    }
    
    
    // 发送ESP8266_WIFI指令，等待返回OK，如果返回错误，则发送AT+CWJAP failed，并返回ESP8266_ERROR
#if DEBUG
    DEBUG_LOG("ESP8266_WIFI\n");
#endif
    while(ESP8266_SendCmd(ESP8266_WIFI, "OK", 5000) != ESP8266_OK)
    {
#if DEBUG
		
        DEBUG_LOG("ESP8266_WIFI ERROR\n");
		DEBUG_LOG("rx_buffer:%s \n",esp8266_rx_buffer);
#endif
        HAL_Delay(1000);
        return ESP8266_ERROR;
    }

    // 发送ESP8266_TCP指令，等待返回OK，如果返回错误，则发送AT+CIPSTART failed，并返回ESP8266_ERROR
#if DEBUG
    DEBUG_LOG("ESP8266_TCP\n");
#endif
    while(ESP8266_SendCmd(ESP8266_TCP, "OK", 5000) != ESP8266_OK)
    {
#if DEBUG
        DEBUG_LOG("ESP8266_TCP ERROR\n");
#endif
        HAL_Delay(1000);
        return ESP8266_ERROR;
    }


	//Debug_usart3_Send((uint8_t*)esp8266_rx_buffer,3);
    // 发送ESP8266 Init OK
    DEBUG_LOG("ESP8266 Init OK\n");

    // 返回ESP8266_OK
    return ESP8266_OK;
}

/*
 * @brief   向ESP8266发送数据
 * @param   *data 数据地址
 * 			len 数据长度
 * @retval  ESP8266状态
 */
ESP8266_StatusTypeDef_t ESP8266_SendData(char *data,uint16_t len)
{
	char cmdbuf[32] = {0};
	ESP8266_Clear();
	sprintf(cmdbuf,"AT+CIPSEND=%d\r\n", len);
	if(ESP8266_SendCmd(cmdbuf,">",1000) == ESP8266_OK)
	{
		wifi_usart2_Send((uint8_t*)data,len);
		return ESP8266_OK;
	}
	return ESP8266_ERROR;
}

/*
 * @brief   向ESP8266发送数据
 *			不同的网络设备返回的格式不同
 *			如EPS8266 "+IPD,x:yyy" x数据长度，yyy数据内容			
 * @param   waittime 等待时间
 * @retval  ESP8266状态
 */
unsigned char* ESP8266_GetIPD(unsigned short waittime)
{
	char *ptrIPD = NULL;
	do
	{
		/*接收完成*/
		if(ESP8266_WaitReceive() == ESP8266_OK)
		{
			/*搜索IPD头*/
#if DEBUG
			//DEBUG_LOG("rx_buffer:%s\n",esp8266_rx_buffer);
#endif
			ptrIPD = strstr((char *)esp8266_rx_buffer,"IPD,");
			if(ptrIPD == NULL)
			{
#if DEBUG
				DEBUG_LOG("IPD not found!\n");
				
#endif
			}
			else
			{
				ptrIPD = strchr(ptrIPD,':');
				if(ptrIPD != NULL)
				{
					ptrIPD++;
					
					return (unsigned char*)(ptrIPD);
				}
				else return NULL;
			
			}
		
		}
		HAL_Delay(5);
		waittime--;
	}while(waittime > 0);
	
	return NULL;
}


// UART1 wifi 接收完成回调函数
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (USART2 == huart->Instance)
    {
        taskENTER_CRITICAL(); // FreeRTOS 临界区保护
        if (esp8266_cnt < ESP8266_RX_BUFFER_SIZE - 1)
        {
            esp8266_rx_buffer[esp8266_cnt++] = huart->Instance->DR;
        }
        taskEXIT_CRITICAL();

        wifi_usart2_Receive(&esp8266_rx_buffer[esp8266_cnt], 1);
    }
}

