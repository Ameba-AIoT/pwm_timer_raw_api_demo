* [中文说明](./README_CN.md)

### RTL8721Dx PWM Timer Example — Safe ISR Callback & RAW GPIO/Timer API (FreeRTOS)

🔹 This example shows how to use the **general timer RAW APIs (RTIM\_… )** together with GPIO on RTL8721Dx series SoCs to control 3 LED channels

- 📎 EVB purchase links:  
  - [🛒 Taobao](https://item.taobao.com/item.htm?id=904981157046)  
  - [📦 Amazon](https://www.amazon.com/-/zh/dp/B0FB33DT2C/)  
- 📄 [Chip & module information](https://aiot.realmcu.com/cn/module/index.html)  
- 📚 [PWM documentation (Timer peripheral)](https://aiot.realmcu.com/zh/latest/rtos/peripherals/timer/index.html#pwm-timer)  
- 🎬 [Ameba SoC PWM tutorial video](https://aiot.realmcu.com/zh/center/videos/detail/29?theme_id=3)

### Features

- **TIMER1** (periodic mode)  
  - Configured as a periodic timer using `RTIM_TimeBaseInit` + `RTIM_Cmd(ENABLE)` with period `PWM_TIME_US`  
  - When the interrupt occurs, the ISR only sets `time1_expired` to 1 and clears the interrupt pending bit  
  - In the main loop, after detecting the flag:
    - Toggle the GPIO corresponding to `LED1_PIN`  
    - Print `"timer1 timeout..."` log  

- **TIMER2** (one-shot mode)  
  - Configured as one-shot using `RTIM_TimeBaseInit`, with timeout corresponding to `LED2_TIME_US`  
  - In the ISR, set `time2_expired` to 1 and clear the interrupt pending bit  
  - In the main loop, after detecting the flag:
    - Toggle the GPIO corresponding to `LED2_PIN`  
    - Print `"timer2 timeout..."` log  
    - Call `RTIM_Cmd(TIMx[GTIMER_2], ENABLE)` again to restart TIMER2, so that LED2 keeps blinking  

- **TIMER3** (one-shot mode)  
  - Similar to TIMER2, but uses `LED3_PIN` with timeout `LED3_TIME_US`  
  - In the ISR, set `time3_expired` to 1 and clear the interrupt pending bit  
  - In the main loop, after detecting the flag:
    - Toggle the GPIO corresponding to `LED3_PIN`  
    - Print `"timer3 timeout..."` log  
    - Call `RTIM_Cmd(TIMx[GTIMER_3], ENABLE)` to restart TIMER3  


### Hardware Setup

1️⃣ **Required components**

- 3 LEDs (optional)  
  ⚠️ Most EVBs already include one or more on-board LEDs. You can optionally connect 3 external LEDs to demonstrate 3 timer channels separately.

2️⃣ **Wiring (when using external LEDs)**

- Connect LEDs to the corresponding pins and GND on the EVB (pins are defined in `pwm_timer_raw_RGB.h`, for example):

  ```c
  #define LED1_PIN _PA_14
  #define LED2_PIN _PA_15
  #define LED3_PIN _PA_16
  ```

- Recommended wiring:  
  - `LED1_PIN` → LED1 anode (+)  
  - `LED2_PIN` → LED2 anode (+)  
  - `LED3_PIN` → LED3 anode (+)  
  - Each LED cathode (−) connected to `GND` through a current-limiting resistor  

> If you only want to test the software logic, you can use on-board LEDs directly without external components.

### Getting Started

1️⃣ **Initialize SDK environment**
   - Set the path of `env.sh` (`env.bat`) and run: `source {sdk}/env.sh`
   - Replace `{sdk}` with the absolute path to the root directory of the [ameba-rtos SDK](https://github.com/Ameba-AIoT/ameba-rtos).
   
   ⚡ **Note**: This example only supports SDK version **≥ v1.2**.

2️⃣ **Build**
   ```bash
   source env.sh
   ameba.py build
   ```

3️⃣ **Flash to FLASH**
   ```bash
   ameba.py flash --p COMx --image km4_boot_all.bin 0x08000000 0x8014000 --image km0_km4_app.bin 0x08014000 0x8200000
   ```
   ⚠️ Note: You can also flash the prebuilt bin files provided in the project directory by:
   ```bash
   ameba.py flash --p COMx --image ../km4_boot_all.bin 0x08000000 0x8014000 --image ../km0_km4_app.bin 0x08014000 0x8200000
   ```

4️⃣ **Open serial monitor**
   -  `ameba.py monitor --port COMx --b 1500000`

5️⃣ **Press the EVB RST button and observe the log output**

---

### Log Example

```bash
Logs:
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
