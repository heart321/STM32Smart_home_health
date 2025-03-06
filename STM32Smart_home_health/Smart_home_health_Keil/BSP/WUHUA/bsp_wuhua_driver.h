#ifndef __BSP_WUHUA_H__
#define __BSP_WUHUA_H__
#include "stm32f2xx_hal.h"

void atomizer_init(void);
void atomizer_on(void);
void atomizer_off(void);
int atomizer_read(void);
#endif
