#ifndef __BSP_ESP8266_DRIVER_H__
#define __BSP_ESP8266_DRIVER_H__

#include "stm32f2xx_hal.h"

#define ESP8266_RX_BUFFER_SIZE 256

/*WIFI 名称 密码 
云服务器的IP Port*/
#define ESP8266_WIFI    "AT+CWJAP=\"xiaoxi\",\"12345678\"\r\n"
#define ESP8266_TCP     "AT+CIPSTART=\"TCP\",\"47.109.187.11\",1883\r\n" 


/*ESP8266状态*/
typedef enum {
    ESP8266_OK = 0,
    ESP8266_ERROR,
    ESP8266_TIMEOUT
} ESP8266_StatusTypeDef_t;


void ESP8266_Clear(void);

ESP8266_StatusTypeDef_t ESP8266_WaitReceive(void);

ESP8266_StatusTypeDef_t ESP8266_SendCmd(char *cmd,char *res, uint32_t waittime);

ESP8266_StatusTypeDef_t ESP8266_Connect(void);

ESP8266_StatusTypeDef_t ESP8266_SendData(char *data,uint16_t len);

unsigned char* ESP8266_GetIPD(unsigned short waittime);


#endif /* __BSP_ESP8266_DRIVER_H__ */
