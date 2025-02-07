#ifndef __APP_LM2904_TASK_H__
#define __APP_LM2904_TASK_H__

typedef struct
{
    uint16_t adc_value;
    float db_value;
}dbData_t;

void lm2904_task(void *pvParameters);










#endif
