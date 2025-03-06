/*************************************************************
	*	文件名： 	Cloud.c
	*
	*	说明： 		与Cloud平台的数据交互接口层
	*
	*	修改记录：	V1.0：协议封装、返回判断都在同一个文件，并且不同协议接口不同。
	*				V1.1：提供统一接口供应用层使用，根据不同协议文件来封装协议相关的内容。
	************************************************************
*/

//单片机头文件
#include "stm32f2xx.h"

//网络设备
#include "bsp_esp8266_driver.h"

//协议文件
#include "Cloud.h"
#include "Mqttkit.h"

//硬件驱动
#include "bsp_usart_driver.h"
#include "bsp_sg90_driver.h"

//用户文件
#include "app_revData_task.h"

//C库
#include <string.h>
#include <stdio.h>
#include "cJSON.h"

#define PROID	   	"xchen"
#define AUTH_INFO	"3321"
#define DEVID		"Smart_home"

extern unsigned char esp8266_buf[128];
extern uint8_t alarmFlag;


//==========================================================
//	函数名称：	Cloud_DevLink
//
//	函数功能：	与Cloud创建连接
//
//	入口参数：	无
//
//	返回参数：	1-成功	0-失败
//
//	说明：		与Cloud平台建立连接
//==========================================================
_Bool Cloud_DevLink(void) {
    MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};
    unsigned char *dataPtr;
    _Bool status = 1;
#if DEBUG
    DEBUG_LOG("Cloud_DevLink\nPROID: %s, AUIF: %s, DEVID:%s\n", PROID, AUTH_INFO, DEVID);
#endif
    if (MQTT_PacketConnect(PROID, AUTH_INFO, DEVID, 60, 1, MQTT_QOS_LEVEL0, NULL, NULL, 0, &mqttPacket) == 0) { // Keepalive 60秒
//#if DEBUG
//        DEBUG_LOG("CONNECT 数据包: ");
//        for (uint32_t i = 0; i < mqttPacket._len; i++) {
//            DEBUG_LOG("%02X ", mqttPacket._data[i]);
//        }
//        DEBUG_LOG("\n");
//#endif
        ESP8266_SendData((char*)mqttPacket._data, mqttPacket._len);
        dataPtr = ESP8266_GetIPD(100);
        if (dataPtr != NULL) {
            if (MQTT_UnPacketRecv(dataPtr) == MQTT_PKT_CONNACK) {
                switch (MQTT_UnPacketConnectAck(dataPtr)) {
                    case 0: DEBUG_LOG("MQTT Broker 连接成功!\n"); status = 0; break;
                    default: DEBUG_LOG("MQTT连接失败：未知错误\n"); break;
                }
            }
        }
        MQTT_DeleteBuffer(&mqttPacket);
    }
    return status;
}

//==========================================================
//	函数名称：	Cloud_Subscribe
//
//	函数功能：	订阅
//
//	入口参数：	topics：订阅的topic
//				topic_cnt：topic个数
//
//	返回参数：	SEND_TYPE_OK-成功	SEND_TYPE_SUBSCRIBE-需要重发
//
//	说明：		
//==========================================================
Cloud_Status_t Cloud_Subscribe(const char *topics[], unsigned char topic_cnt) {
    MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};
    if (MQTT_PacketSubscribe(MQTT_SUBSCRIBE_ID, MQTT_QOS_LEVEL0, topics, topic_cnt, &mqttPacket) == 0) { // QoS 1
#if DEBUG
        DEBUG_LOG("SUBSCRIBE 数据包: ");
        for (uint32_t i = 0; i < mqttPacket._len; i++) {
            DEBUG_LOG("%02X ", mqttPacket._data[i]);
        }
        DEBUG_LOG("\n");
#endif
        ESP8266_SendData((char*)mqttPacket._data, mqttPacket._len);
        MQTT_DeleteBuffer(&mqttPacket);
        return Cloud_Ok;
    }
    return Cloud_ERROR;
}

//==========================================================
//	函数名称：	Cloud_Publish
//
//	函数功能：	发布消息
//
//	入口参数：	topic：发布的主题
//				msg：消息内容
//
//	返回参数：	SEND_TYPE_OK-成功	SEND_TYPE_PUBLISH-需要重送
//
//	说明：		
//==========================================================
Cloud_Status_t Cloud_Publish(const char *topic, const char *msg)
{

	MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};							//协议包
#if DEBUG	
	DEBUG_LOG("Publish Topic: %s, Msg: %s\r\n", topic, msg);
#endif	
	if(MQTT_PacketPublish(MQTT_PUBLISH_ID, topic, msg, strlen(msg), MQTT_QOS_LEVEL0, 0, 1, &mqttPacket) == 0)
	{
		ESP8266_SendData((char*)mqttPacket._data, mqttPacket._len);					//向平台发送订阅请求
		
		MQTT_DeleteBuffer(&mqttPacket);											//删包
		return Cloud_Ok;
	}
	return Cloud_ERROR;
}

//==========================================================
//	函数名称：	Cloud_RevPro
//
//	函数功能：	平台返回数据检测
//
//	入口参数：	dataPtr：平台返回的数据
//
//	返回参数：	无
//
//	说明：		
//==========================================================
void Cloud_RevPro(unsigned char *cmd)
{
	
	MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};								//协议包
	
	char *req_payload = NULL;
	char *cmdid_topic = NULL;
	
	unsigned short topic_len = 0;
	unsigned short req_len = 0;
	
	unsigned char type = 0;
	unsigned char qos = 0;
	static unsigned short pkt_id = 0;
	
	short result = 0;

	char *dataPtr = NULL;
	char numBuf[10];
	int num = 0;
	
	cJSON *json;
	
	type = MQTT_UnPacketRecv(cmd);
	switch(type)
	{
		case MQTT_PKT_CMD:															//命令下发
			
			result = MQTT_UnPacketCmd(cmd, &cmdid_topic, &req_payload, &req_len);	//解出topic和消息体
			if(result == 0)
			{
#if DEBUG
				DEBUG_LOG("cmdid: %s, req: %s, req_len: %d\r\n", cmdid_topic, req_payload, req_len);
#endif
				MQTT_DeleteBuffer(&mqttPacket);									//删包
			}
		
		break;
			
		case MQTT_PKT_PUBLISH:														//接收的Publish消息
		
			result = MQTT_UnPacketPublish(cmd, &cmdid_topic, &topic_len, &req_payload, &req_len, &qos, &pkt_id);
			if(result == 0)
			{
#if DEBUG
				DEBUG_LOG("topic: %s, topic_len: %d, payload: %s, payload_len: %d\r\n",
																cmdid_topic, topic_len, req_payload, req_len);
#endif	
				// 对数据包req_payload进行JSON格式解析
				json = cJSON_Parse(req_payload);
				if (!json)printf("Error before: [%s]\n",cJSON_GetErrorPtr());
				else
				{
					/*处理json数据*/
					revData_Json(json);
				}
				cJSON_Delete(json);
			}
		
		break;
			
		case MQTT_PKT_PUBACK:														//发送Publish消息，平台回复的Ack
		
			if(MQTT_UnPacketPublishAck(cmd) == 0)
#if DEBUG
				DEBUG_LOG("Tips:	MQTT Publish Send OK\r\n");
#endif
			
		break;
			
		case MQTT_PKT_PUBREC:														//发送Publish消息，平台回复的Rec，设备需回复Rel消息
		
			if(MQTT_UnPacketPublishRec(cmd) == 0)
			{
#if DEBUG
				DEBUG_LOG("Tips:	Rev PublishRec\r\n");
#endif
				if(MQTT_PacketPublishRel(MQTT_PUBLISH_ID, &mqttPacket) == 0)
				{
#if DEBUG
					DEBUG_LOG("Tips:	Send PublishRel\r\n");
#endif
					ESP8266_SendData((char *)mqttPacket._data, mqttPacket._len);
					MQTT_DeleteBuffer(&mqttPacket);
				}
			}
		
		break;
			
		case MQTT_PKT_PUBREL:														//收到Publish消息，设备回复Rec后，平台回复的Rel，设备需再回复Comp
			
			if(MQTT_UnPacketPublishRel(cmd, pkt_id) == 0)
			{
#if DEBUG				
				DEBUG_LOG("Tips:	Rev PublishRel\r\n");
#endif				
				if(MQTT_PacketPublishComp(MQTT_PUBLISH_ID, &mqttPacket) == 0)
				{
#if DEBUG					
					DEBUG_LOG("Tips:	Send PublishComp\r\n");
#endif					
					ESP8266_SendData((char *)mqttPacket._data, mqttPacket._len);
					MQTT_DeleteBuffer(&mqttPacket);
				}
			}
		
		break;
		
		case MQTT_PKT_PUBCOMP:														//发送Publish消息，平台返回Rec，设备回复Rel，平台再返回的Comp
		
			if(MQTT_UnPacketPublishComp(cmd) == 0)
			{
#if DEBUG				
				DEBUG_LOG("Tips:	Rev PublishComp\r\n");
#endif			
			}
		
		break;
			
		case MQTT_PKT_SUBACK:														//发送Subscribe消息的Ack
		
			if(MQTT_UnPacketSubscribe(cmd) == 0)
#if DEBUG
				DEBUG_LOG("Tips:	MQTT Subscribe OK\r\n");
		
			else
				
				DEBUG_LOG("Tips:	MQTT Subscribe Err\r\n");
#endif
		
		break;
			
		case MQTT_PKT_UNSUBACK:														//发送UnSubscribe消息的Ack
		
			if(MQTT_UnPacketUnSubscribe(cmd) == 0)
#if DEBUG
				DEBUG_LOG("Tips:	MQTT UnSubscribe OK\r\n");

			else

				DEBUG_LOG("Tips:	MQTT UnSubscribe Err\r\n");
#endif
		
		break;
		
		default:
			result = -1;
		break;
	}
	
	ESP8266_Clear();									//清空缓存
	
	if(result == -1)
		return;
	
	dataPtr = strchr(req_payload, '}');					//搜索'}'

	if(dataPtr != NULL && result != -1)					//如果找到了
	{
		dataPtr++;
		
		while(*dataPtr >= '0' && *dataPtr <= '9')		//判断是否是下发的命令控制数据
		{
			numBuf[num++] = *dataPtr++;
		}
		
		num = atoi((const char *)numBuf);				//转为数值形式
		
	}

	if(type == MQTT_PKT_CMD || type == MQTT_PKT_PUBLISH)
	{
		MQTT_FreeBuffer(cmdid_topic);
		MQTT_FreeBuffer(req_payload);
	}

}
