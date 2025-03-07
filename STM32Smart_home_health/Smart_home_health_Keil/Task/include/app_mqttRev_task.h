#ifndef __REVDATA_H__
#define __REVDATA_H__
#include "Cloud.h"
#include "cJSON.h"


void mqtt_rev_task(void *pvParameters);
void revData_Json(cJSON *json);




#endif
