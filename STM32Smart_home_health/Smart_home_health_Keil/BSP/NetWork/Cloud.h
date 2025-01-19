#ifndef _Cloud_H_
#define _Cloud_H_


/*连接平台的状态*/
typedef enum
{
	Cloud_Ok =0,
	Cloud_ERROR
}Cloud_Status_t;




_Bool Cloud_DevLink(void);

Cloud_Status_t Cloud_Subscribe(const char *topics[], unsigned char topic_cnt);

Cloud_Status_t Cloud_Publish(const char *topic, const char *msg);

void Cloud_RevPro(unsigned char *cmd);

#endif
