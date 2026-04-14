#ifndef __PWM_TIMER_RAW_RGB__
#define __PWM_TIMER_RAW_RGB__
#include "device.h"
#include "os_wrapper.h"
#include <stdio.h>
#include "main.h"

#define TAG         "PWM_TIMER_RAM_DEMO"
//定义三个gpio对应的evb pin idex
#define LED1_PIN _PA_14
#define LED2_PIN _PA_15
#define LED3_PIN _PA_16

//定义三个timer
#define GTIMER_1    1
#define GTIMER_2    2
#define GTIMER_3    3

#define PWM_TIME_US  2000000
#define LED2_TIME_US PWM_TIME_US / 8
#define LED3_TIME_US PWM_TIME_US / 4

void gpio_led_init(void);
void pwm_timer_raw_RGB(void);
u32 timer1_timeout_handler(void *id);
u32 timer2_timeout_handler(void *id);
u32 timer3_timeout_handler(void *id);

#endif
