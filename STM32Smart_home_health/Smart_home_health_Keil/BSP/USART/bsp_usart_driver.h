#ifndef __BSP_USART_DRIVER_H__
#define __BSP_USART_DRIVER_H__

#include "stm32f2xx.h"
#include <stdio.h>

#define DEBUG  1 //调试信息
#define DEBUG_LOG(format, ...) printf(format, ##__VA_ARGS__)


/*串口1发送数据*/
void wifi_usart2_Send(uint8_t *data,uint16_t len);
/*串口1接收数据*/
void wifi_usart2_Receive(uint8_t *data,uint16_t len);


/*串口1 发送数据*/
void Debug_usart1_Send(uint8_t *data,uint16_t len);


#endif /* __BSP_USART_DRIVER_H__ */
