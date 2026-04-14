* [English Version](./README.md)

### RTL8721Dx PWM Timer 示例 — 安全中断回调 & RAW GPIO/Timer API（FreeRTOS）
🔹 本示例展示如何在 RTL8721Dx 系列 SoC 上，使用 **通用定时器 RAW API（RTIM\_…）** 配合 GPIO 控制 3 路 LED 闪烁

- 📎 开发板购买链接：  
  - [🛒 Taobao](https://item.taobao.com/item.htm?id=904981157046)  
  - [📦 Amazon](https://www.amazon.com/-/zh/dp/B0FB33DT2C/)  
- 📄 [芯片与模组详情](https://aiot.realmcu.com/cn/module/index.html)  
- 📚 [PWM 文档（Timer 外设）](https://aiot.realmcu.com/zh/latest/rtos/peripherals/timer/index.html#pwm-timer)  
- 🎬 [Ameba SoC PWM 学习视频](https://aiot.realmcu.com/zh/center/videos/detail/29?theme_id=3)

### 功能特点

- **TIMER1**（周期模式）  
  - 使用 `RTIM_TimeBaseInit` + `RTIM_Cmd(ENABLE)` 配置为周期定时，周期为 `PWM_TIME_US`  
  - 中断发生时，只在 ISR 中将 `time1_expired` 置 1，并清除中断挂起标志  
  - 主循环检测到标志后：
    - 翻转 `LED1_PIN` 对应 GPIO  
    - 打印 `"timer1 timeout..."` 日志  

- **TIMER2**（one-shot 模式）  
  - 使用 `RTIM_TimeBaseInit` 配置 one-shot，超时时间对应 `LED2_TIME_US`  
  - ISR 中将 `time2_expired` 置 1，并清除中断挂起标志  
  - 主循环检测到标志后：
    - 翻转 `LED2_PIN` 对应 GPIO  
    - 打印 `"timer2 timeout..."` 日志  
    - 再次调用 `RTIM_Cmd(TIMx[GTIMER_2], ENABLE)` 重新启动 TIMER2，实现循环闪烁  

- **TIMER3**（one-shot 模式）  
  - 与 TIMER2 类似，对应 `LED3_PIN`，超时时间为 `LED3_TIME_US`  
  - ISR 中将 `time3_expired` 置 1，并清除中断挂起标志  
  - 主循环检测到标志后：
    - 翻转 `LED3_PIN` 对应 GPIO  
    - 打印 `"timer3 timeout..."` 日志  
    - 调用 `RTIM_Cmd(TIMx[GTIMER_3], ENABLE)` 重新启动 TIMER3  


### 搭建硬件环境

1️⃣ **所需组件**

- 3 个 LED（可选）  
  ⚠️ 大部分 EVB 已集成 1 颗或多颗 LED，你也可以外接 3 个 LED 分别演示 3 路定时器效果。

2️⃣ **连线说明（如使用外接 LED）**

- 将 LED 分别连接到开发板对应引脚与 GND（具体引脚在 `pwm_timer_raw_RGB.h` 中定义，例如）：

  ```c
  #define LED1_PIN _PA_14
  #define LED2_PIN _PA_15
  #define LED3_PIN _PA_16
  ```

- 建议连接方式：  
  - `LED1_PIN` —— `LED1 正极`  
  - `LED2_PIN` —— `LED2 正极`  
  - `LED3_PIN` —— `LED3 正极`  
  - 每个 LED 负极串联限流电阻后接 `GND`  

> 仅体验软件逻辑时，可直接使用板载 LED，不接外部器件。

### 快速开始

1️⃣ **初始化SDK环境**
   - 设置 `env.sh` (`env.bat`) 路径：`source {sdk}/env.sh`
   - 将 `{sdk}` 替换为 [ameba-rtos SDK](https://github.com/Ameba-AIoT/ameba-rtos) 根目录的绝对路径。
   
   ⚡ **注意**：本示例仅支持 SDK 版本 **≥ v1.2**。

2️⃣ **编译**
   ```bash
   source env.sh
   ameba.py build
   ```

3️⃣ **烧录 FLASH**
   ```bash
   ameba.py flash --p COMx --image km4_boot_all.bin 0x08000000 0x8014000 --image km0_km4_app.bin 0x08014000 0x8200000
   ```
   ⚠️ 注意：项目目录中提供的预编译 bin 文件也可以用如下方式烧录：
   ```bash
   ameba.py flash --p COMx --image ../km4_boot_all.bin 0x08000000 0x8014000 --image ../km0_km4_app.bin 0x08014000 0x8200000
   ```

4️⃣ **打开串口监视**
   -  `ameba.py monitor --port COMx --b 1500000`

5️⃣ **点击 EVB RST 按钮后，观察日志输出**

---

### 日志示例

```bash
日志：

   [MONITOR] 2026-04-13 18:20:46.400 ROM:[V1.1]
   [MONITOR] 2026-04-13 18:20:46.401 FLASH RATE:1, Pinmux:1
   [MONITOR] 2026-04-13 18:20:46.401 IMG1(OTA1) VALID, ret: 0
   [MONITOR] 2026-04-13 18:20:46.401 IMG1 ENTRY[f800779:0]
   [MONITOR] 2026-04-13 18:20:46.401 [BOOT-I] KM4 BOOT REASON 0: Initial Power on
   [MONITOR] 2026-04-13 18:20:46.401 [BOOT-I] KM4 CPU CLK: 240000000 Hz
   [MONITOR] 2026-04-13 18:20:46.401 [BOOT-I] KM0 CPU CLK: 96000000 Hz
   [MONITOR] 2026-04-13 18:20:46.401 [BOOT-I] PSRAM Ctrl CLK: 240000000 Hz
   [MONITOR] 2026-04-13 18:20:46.411 [BOOT-I] IMG1 ENTER MSP:[30009FDC]
   [MONITOR] 2026-04-13 18:20:46.411 [BOOT-I] Build Time: Apr 13 2026 16:29:22
   [MONITOR] 2026-04-13 18:20:46.411 [BOOT-I] IMG1 SECURE STATE: 1
   [MONITOR] 2026-04-13 18:20:46.411 [FLASH-I] FLASH CLK: 80000000 Hz
   [MONITOR] 2026-04-13 18:20:46.411 [FLASH-I] Flash ID: 85-20-16 (Capacity: 32M-bit)
   [MONITOR] 2026-04-13 18:20:46.411 [FLASH-I] Flash Read 4IO
   [MONITOR] 2026-04-13 18:20:46.411 [FLASH-I] FLASH HandShake[0x2 OK]
   [MONITOR] 2026-04-13 18:20:46.421 [BOOT-I] KM0 XIP IMG[0c000000:8600]
   [MONITOR] 2026-04-13 18:20:46.421 [BOOT-I] KM0 SRAM[20068000:860]
   [MONITOR] 2026-04-13 18:20:46.421 [BOOT-I] KM0 PSRAM[0c008e60:20]
   [MONITOR] 2026-04-13 18:20:46.421 [BOOT-I] KM0 ENTRY[20004d00:60]
   [MONITOR] 2026-04-13 18:20:46.421 [BOOT-I] KM4 XIP IMG[0e000000:18820]
   [MONITOR] 2026-04-13 18:20:46.421 [BOOT-I] KM4 SRAM[2000b000:520]
   [MONITOR] 2026-04-13 18:20:46.421 [BOOT-I] KM4 PSRAM[0e018d40:20]
   [MONITOR] 2026-04-13 18:20:46.421 [BOOT-I] KM4 ENTRY[20004d80:40]
   [MONITOR] 2026-04-13 18:20:46.421 [BOOT-I] IMG2 BOOT from OTA 1, Version: 1.1
   [MONITOR] 2026-04-13 18:20:46.421 [BOOT-I] Image2Entry @ 0xe007a65 ...
   [MONITOR] 2026-04-13 18:20:46.421 [APP-I] KM4 APP STA[LOCKS-IRT
   [MONITOR] 2026-04-13 18:20:46.421 [AP] KM0 inP-I] VTOit_retarR: 30007get_lock000, VTOR_NS:300s
   [MONITOR] 2026-04-13 18:20:46.421 07000
   [MONITOR] 2026-04-13 18:20:46.421 [APP-I] VTOR: 30007000, VTOR_NS:30007000
   [MONITOR] 2026-04-13 18:20:46.421 [APP-I] IMG2 SECURE STATE:[MAIN-I] 1
   [MONITOR] 2026-04-13 18:20:46.421  IWDG refresh on!
   [MONITOR] 2026-04-13 18:20:46.421 [MAIN-I] KM0 OS START
   [MONITOR] 2026-04-13 18:20:46.421 [CLK-I] [CAL4M]: delta:2 target:320 PPM: 6250 PPM_Limit:30000
   [MONITOR] 2026-04-13 18:20:46.431 [CLK-I] [CAL131K]: delta:12 target:2441 PPM: 4916 PPM_Limit:30000
   [MONITOR] 2026-04-13 18:20:46.431 [LOCKS-I] KM4 init_retarget_locks
   [MONITOR] 2026-04-13 18:20:46.431 [APP-I] BOR arises when supply voltage decreases under 2.57V and recovers above 2.7V.
   [MONITOR] 2026-04-13 18:20:46.431 [MAIN-I] KM4 MAIN
   [MONITOR] 2026-04-13 18:20:46.431 [VER-I] AMEBA-RTOS SDK VERSION: 1.3.0
   [MONITOR] 2026-04-13 18:20:46.431 [MAIN-I] File System Init Success
   [MONITOR] 2026-04-13 18:20:46.441 [MAIN-I] KM4 START SCHEDULER
   [MONITOR] 2026-04-13 18:20:46.441 [PWM_TIMER_RAM_DEMO-I] gpio_led_setup ready!
   [MONITOR] 2026-04-13 18:20:49.440 [PWM_TIMER_RAM_DEMO-I] pwm_timer_raw_RGB demo start!
   [MONITOR] 2026-04-13 18:20:49.692 [PWM_TIMER_RAM_DEMO-I] timer2 timeout handle, led_15 timer:250 ms
   [MONITOR] 2026-04-13 18:20:49.943 [PWM_TIMER_RAM_DEMO-I] timer2 timeout handle, led_15 timer:250 ms
   [MONITOR] 2026-04-13 18:20:49.943 [PWM_TIMER_RAM_DEMO-I] timer3 timeout handle, led_16 timer:500 ms
   [MONITOR] 2026-04-13 18:20:50.195 [PWM_TIMER_RAM_DEMO-I] timer2 timeout handle, led_15 timer:250 ms
   [MONITOR] 2026-04-13 18:20:50.436 [PWM_TIMER_RAM_DEMO-I] timer2 timeout handle, led_15 timer:250 ms
   [MONITOR] 2026-04-13 18:20:50.436 [PWM_TIMER_RAM_DEMO-I] timer3 timeout handle, led_16 timer:500 ms
   [MONITOR] 2026-04-13 18:20:50.688 [PWM_TIMER_RAM_DEMO-I] timer2 timeout handle, led_15 timer:250 ms
   [MONITOR] 2026-04-13 18:20:50.940 [PWM_TIMER_RAM_DEMO-I] timer2 timeout handle, led_15 timer:250 ms
   [MONITOR] 2026-04-13 18:20:50.940 [PWM_TIMER_RAM_DEMO-I] timer3 timeout handle, led_16 timer:500 ms
   [MONITOR] 2026-04-13 18:20:51.191 [PWM_TIMER_RAM_DEMO-I] timer2 timeout handle, led_15 timer:250 ms
   [MONITOR] 2026-04-13 18:20:51.443 [PWM_TIMER_RAM_DEMO-I] timer1 timeout handle, led_14 timer:2000 ms
   [MONITOR] 2026-04-13 18:20:51.443 [PWM_TIMER_RAM_DEMO-I] timer2 timeout handle, led_15 timer:250 ms
   [MONITOR] 2026-04-13 18:20:51.443 [PWM_TIMER_RAM_DEMO-I] timer3 timeout handle, led_16 timer:500 ms
   [MONITOR] 2026-04-13 18:20:51.695 [PWM_TIMER_RAM_DEMO-I] timer2 timeout handle, led_15 timer:250 ms
   [MONITOR] 2026-04-13 18:20:51.936 [PWM_TIMER_RAM_DEMO-I] timer2 timeout handle, led_15 timer:250 ms
   [MONITOR] 2026-04-13 18:20:51.936 [PWM_TIMER_RAM_DEMO-I] timer3 timeout handle, led_16 timer:500 ms
   [MONITOR] 2026-04-13 18:20:52.188 [PWM_TIMER_RAM_DEMO-I] timer2 timeout handle, led_15 timer:250 ms
   [MONITOR] 2026-04-13 18:20:52.439 [PWM_TIMER_RAM_DEMO-I] timer2 timeout handle, led_15 timer:250 ms
   [MONITOR] 2026-04-13 18:20:52.439 [PWM_TIMER_RAM_DEMO-I] timer3 timeout handle, led_16 timer:500 ms
   [MONITOR] 2026-04-13 18:20:52.691 [PWM_TIMER_RAM_DEMO-I] timer2 timeout handle, led_15 timer:250 ms
   [MONITOR] 2026-04-13 18:20:52.943 [PWM_TIMER_RAM_DEMO-I] timer2 timeout handle, led_15 timer:250 ms
   [MONITOR] 2026-04-13 18:20:52.943 [PWM_TIMER_RAM_DEMO-I] timer3 timeout handle, led_16 timer:500 ms
   [MONITOR] 2026-04-13 18:20:53.194 [PWM_TIMER_RAM_DEMO-I] timer2 timeout handle, led_15 timer:250 ms
   [MONITOR] 2026-04-13 18:20:53.436 [PWM_TIMER_RAM_DEMO-I] timer1 timeout handle, led_14 timer:2000 ms
...
```