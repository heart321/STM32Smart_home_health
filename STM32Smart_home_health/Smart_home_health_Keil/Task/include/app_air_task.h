#ifndef __APP_AIQ_TASK_H__
#define __APP_AIQ_TASK_H__

typedef struct
{
    float TVOC;
    float CO2;
    float HCHO;
    int   PM25;
}airData_t;


void air_task(void *pvParameters);








#endif
