#include "pwm_timer_raw_RGB.h"
#include "FreeRTOS.h"
#include "task.h"

void app_example(void)
{
    if (rtos_task_create( NULL, "pwm_timer_raw_RGB", (rtos_task_t)pwm_timer_raw_RGB, NULL, 1024, tskIDLE_PRIORITY) != RTK_SUCCESS )
    {
        RTK_LOGE(TAG, "pwm_timer_raw_RGB failed!\r\n");
        return;
    }   
}
    