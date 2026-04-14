#include "pwm_timer_raw_RGB.h"

// 中断与主循环之间的标志位（ISR 写，主循环读）
volatile uint32_t time1_expired = 0;
volatile uint32_t time2_expired = 0;
volatile uint32_t time3_expired = 0;

/**
 * @brief TIMER1 超时中断回调（周期模式）
  */
u32 timer1_timeout_handler(void *id)
{
    RTIM_TimeBaseInitTypeDef *gtimer = (RTIM_TimeBaseInitTypeDef *)id;

    time1_expired = 1;
   
    RTIM_INTClear(TIMx[gtimer->TIM_Idx]);
    /* make sure all intr pending bits cleared ok, to avoid timeout is not enough in rom code */
    RTIM_INTClear(TIMx[gtimer->TIM_Idx]);

    return 0;
}

/**
 * @brief TIMER2 超时中断回调
 */
u32 timer2_timeout_handler(void *id)
{
    RTIM_TimeBaseInitTypeDef *gtimer = (RTIM_TimeBaseInitTypeDef *)id;
    time2_expired = 1;

    RTIM_INTClear(TIMx[gtimer->TIM_Idx]);
    RTIM_INTClear(TIMx[gtimer->TIM_Idx]);

    return 0;
}

/**
 * @brief TIMER3 超时中断回调
 */
u32 timer3_timeout_handler(void *id)
{
    RTIM_TimeBaseInitTypeDef *gtimer = (RTIM_TimeBaseInitTypeDef *)id;
    time3_expired = 1;

    RTIM_INTClear(TIMx[gtimer->TIM_Idx]);
    RTIM_INTClear(TIMx[gtimer->TIM_Idx]);

    return 0;
}

void gpio_led_init(void)
{
    // 定义三个 GPIO 给 LED 使用
    GPIO_InitTypeDef led1_gpio;
    GPIO_InitTypeDef led2_gpio;
    GPIO_InitTypeDef led3_gpio;

    // 初始化 LED 对应的 GPIO
    led1_gpio.GPIO_Pin = LED1_PIN;
    led2_gpio.GPIO_Pin = LED2_PIN;
    led3_gpio.GPIO_Pin = LED3_PIN;

    led1_gpio.GPIO_Mode = GPIO_Mode_OUT;
    led2_gpio.GPIO_Mode = GPIO_Mode_OUT;
    led3_gpio.GPIO_Mode = GPIO_Mode_OUT;

    GPIO_Init(&led1_gpio);
    GPIO_Init(&led2_gpio);
    GPIO_Init(&led3_gpio);

    RTK_LOGI(TAG, "gpio_led_setup ready!\r\n");

    // LED 全亮一次再全灭，作为上电指示
    GPIO_WriteBit(LED1_PIN, 1);
    GPIO_WriteBit(LED2_PIN, 1);
    GPIO_WriteBit(LED3_PIN, 1);
    rtos_time_delay_ms(1000);

    GPIO_WriteBit(LED1_PIN, 0);
    GPIO_WriteBit(LED2_PIN, 0);
    GPIO_WriteBit(LED3_PIN, 0);
    rtos_time_delay_ms(2000);
}

/**
 * @brief TIM Basic Init Structure Definition
 *
 * typedef struct {
 *     u32 TIM_Prescaler;
 *     u32 TIM_Period;
 *     u32 TIM_UpdateEvent;
 *     u32 TIM_UpdateSource;
 *     u32 TIM_ARRProtection;
 *     u8  TIM_Idx;
 *     u32 TIM_SecureTimer;
 * } RTIM_TimeBaseInitTypeDef;
 */
void pwm_timer_raw_RGB(void)
{
    RTIM_TimeBaseInitTypeDef TIM_InitStruct1;
    RTIM_TimeBaseInitTypeDef TIM_InitStruct2;
    RTIM_TimeBaseInitTypeDef TIM_InitStruct3;

    // 1. 初始化 LED GPIO
    gpio_led_init();

    // 2. 打开三路定时器时钟
    RCC_PeriphClockCmd(APBPeriph_TIMx[GTIMER_1], APBPeriph_TIMx_CLOCK[GTIMER_1], ENABLE);
    RCC_PeriphClockCmd(APBPeriph_TIMx[GTIMER_2], APBPeriph_TIMx_CLOCK[GTIMER_2], ENABLE);
    RCC_PeriphClockCmd(APBPeriph_TIMx[GTIMER_3], APBPeriph_TIMx_CLOCK[GTIMER_3], ENABLE);

    // 3. 配置 TIMER1 为周期模式，用于周期性闪烁 LED1
    RTIM_TimeBaseStructInit(&TIM_InitStruct1);
    TIM_InitStruct1.TIM_Idx    = GTIMER_1;
    TIM_InitStruct1.TIM_Period = PWM_TIME_US / 1000000 * 32768 - 1;

    RTIM_TimeBaseInit(TIMx[GTIMER_1],
                      &TIM_InitStruct1,
                      (IRQn_Type)TIMx_irq[GTIMER_1],
                      (IRQ_FUN)timer1_timeout_handler,
                      (u32)&TIM_InitStruct1);
    RTIM_INTConfig(TIMx[GTIMER_1], TIM_IT_Update, ENABLE);
    RTIM_Cmd(TIMx[GTIMER_1], ENABLE);

    // 4. 配置 TIMER2 / TIMER3 为 one-shot 模式（由主循环控制重新启动）
    // TIMER2
    RTIM_TimeBaseStructInit(&TIM_InitStruct2);
    TIM_InitStruct2.TIM_Idx    = GTIMER_2;
    TIM_InitStruct2.TIM_Period = (u32)((float)LED2_TIME_US / 1000000 * 32768) - 1;

    RTIM_TimeBaseInit(TIMx[GTIMER_2],
                      &TIM_InitStruct2,
                      (IRQn_Type)TIMx_irq[GTIMER_2],
                      (IRQ_FUN)timer2_timeout_handler,
                      (u32)&TIM_InitStruct2);

    // TIMER3
    RTIM_TimeBaseStructInit(&TIM_InitStruct3);
    TIM_InitStruct3.TIM_Idx    = GTIMER_3;
    TIM_InitStruct3.TIM_Period = (u32)((float)LED3_TIME_US / 1000000 * 32768) - 1;

    RTIM_TimeBaseInit(TIMx[GTIMER_3],
                      &TIM_InitStruct3,
                      (IRQn_Type)TIMx_irq[GTIMER_3],
                      (IRQ_FUN)timer3_timeout_handler,
                      (u32)&TIM_InitStruct3);

    // 5. 清零标志并启动 two one-shot timers
    time1_expired = 0;
    time2_expired = 0;
    time3_expired = 0;

    RTIM_INTConfig(TIMx[GTIMER_2], TIM_IT_Update, ENABLE);
    RTIM_Cmd(TIMx[GTIMER_2], ENABLE);

    RTIM_INTConfig(TIMx[GTIMER_3], TIM_IT_Update, ENABLE);
    RTIM_Cmd(TIMx[GTIMER_3], ENABLE);

    RTK_LOGI(TAG, "pwm_timer_raw_RGB demo start!\r\n");

    // 6. 主循环：在任务上下文中处理所有 LED 逻辑 + 打印 + 重新启动 one-shot
    while (1) {

        // TIMER1 周期事件：LED1 闪烁
        if (time1_expired) 
        {
            GPIO_WriteBit(LED1_PIN, !GPIO_ReadDataBit(LED1_PIN));
            RTK_LOGI(TAG,"timer1 timeout handle, led_%lu timer:%lu ms\r\n",
                   (uint32_t)LED1_PIN,
                   (uint32_t)PWM_TIME_US / 1000);
            time1_expired = 0;
        }

        // TIMER2 one-shot：LED2 闪烁 + 重新启动 TIMER2
        if (time2_expired) 
        {
            GPIO_WriteBit(LED2_PIN, !GPIO_ReadDataBit(LED2_PIN));
            RTK_LOGI(TAG,"timer2 timeout handle, led_%lu timer:%lu ms\r\n",
                   (uint32_t)LED2_PIN,
                   (uint32_t)LED2_TIME_US / 1000);

            // 确保计数器重新开始（one-shot -> 手动重新 ENABLE）
            RTIM_Cmd(TIMx[GTIMER_2], ENABLE);

            time2_expired = 0;
        }

        // TIMER3 one-shot：LED3 闪烁 + 重新启动 TIMER3
        if (time3_expired) 
        {
            GPIO_WriteBit(LED3_PIN, !GPIO_ReadDataBit(LED3_PIN));
            RTK_LOGI(TAG,"timer3 timeout handle, led_%lu timer:%lu ms\r\n",
                   (uint32_t)LED3_PIN,
                   (uint32_t)LED3_TIME_US / 1000);

            RTIM_Cmd(TIMx[GTIMER_3], ENABLE);

            time3_expired = 0;
        }
      
        rtos_time_delay_ms(1);
    }

    // 理论上不会执行到这里
    rtos_task_delete(NULL);
}
