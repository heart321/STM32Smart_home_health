#ifndef __BSP_DRV8833_H__
#define __BSP_DRV8833_H__

#include "stm32f2xx_hal.h"

/*电机的方向 */
typedef enum
{
    forword = 0,    // 正转
    reversal        // 反转
}motor_direction_t;


/*drv8833 电机方向 */
void drv8833_motor_direction(motor_direction_t direction);

/*设置电机速度 */
void drv8833_motor_speed(uint32_t speed);

/*电机停止 */
void drv8833_motot_stop(void);


#endif
