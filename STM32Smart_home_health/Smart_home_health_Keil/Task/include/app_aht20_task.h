#ifndef __APP_AHT20_TASK_H__
#define __APP_AHT20_TASK_H__


typedef struct {
    float temperature;
    float humidity;
} weatherData_t;

void aht20_task(void *pvParameters);




#endif /*__APP_AHT20_TASK_H__*/
