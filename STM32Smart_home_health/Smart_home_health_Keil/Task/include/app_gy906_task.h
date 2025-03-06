#ifndef __APP_GY906_H__
#define __APP_GY906_H__


typedef struct
{
    float people_temp;
    float room_temp;
}temperatureData_t;

void gy906_task(void *pvParameters);



#endif
