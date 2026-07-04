// led_task.c
#include "led_task.h"

// 全局变量
volatile uint32_t led_tick = 0;

// 内部显示参数（volatile 确保中断安全）
static volatile uint8_t  s_color_mask       = LED_COLOR_OFF;
static volatile LedMode_t s_mode            = LED_MODE_OFF;

// 自定义闪烁参数
static volatile uint32_t s_on_time_ms       = 500;
static volatile uint32_t s_off_time_ms      = 500;
static volatile uint8_t  s_repeat_count     = 0;         // 0 = 无限
static volatile uint32_t s_pause_after_ms   = 0;


void LED_Init(void)
{
    // 初始全灭
    LED_OFF(LED_R_PORT, LED_R_PIN);
    LED_OFF(LED_G_PORT, LED_G_PIN);
    LED_OFF(LED_B_PORT, LED_B_PIN);

    // 默认配置示例

    LED_SetColorMask(LED_COLOR_CYAN);
    LED_SetMode(LED_MODE_CUSTOM_BLINK);
    LED_SetBlinkParams(100, 100, 2, 900);
}

void LED_Tick_1ms(void)
{
    led_tick++;

    // 先全部关闭
    LED_OFF(LED_R_PORT, LED_R_PIN);
    LED_OFF(LED_G_PORT, LED_G_PIN);
    LED_OFF(LED_B_PORT, LED_B_PIN);

    uint8_t should_on = 0;

    // ── 根据当前模式决定是否点亮 ────────────────────────────────
    switch (s_mode)
    {
        case LED_MODE_OFF:
            break;

        case LED_MODE_SOLID:
            should_on = 1;
            break;

        case LED_MODE_CUSTOM_BLINK:
        {
            uint32_t cycle = s_on_time_ms + s_off_time_ms;
            if (cycle == 0) break;  // 防止除零

            if (s_repeat_count == 0)
            {
                // 无限循环
                uint32_t t = led_tick % cycle;
                should_on = (t < s_on_time_ms);
            }
            else
            {
                // 有限次数 + 暂停
                uint32_t one_blink_time = cycle;
                uint32_t total_blink_time = (uint32_t)s_repeat_count * one_blink_time;
                uint32_t full_cycle = total_blink_time + s_pause_after_ms;

                uint32_t t = led_tick % full_cycle;

                if (t >= total_blink_time)
                {
                    // 暂停阶段，不亮
                    should_on = 0;
                }
                else
                {
                    uint32_t pos = t % cycle;
                    should_on = (pos < s_on_time_ms);
                }
            }
        }
            break;

        default:
            break;
    }

    // ── 输出到 LED ────────────────────────────────────────────────
    if (should_on)
    {
        if (s_color_mask & LED_COLOR_R) LED_ON(LED_R_PORT, LED_R_PIN);
        if (s_color_mask & LED_COLOR_G) LED_ON(LED_G_PORT, LED_G_PIN);
        if (s_color_mask & LED_COLOR_B) LED_ON(LED_B_PORT, LED_B_PIN);
    }
}

// ────────────────────────────────────────────────
// API 实现
// ────────────────────────────────────────────────
void LED_SetColorMask(uint8_t mask)
{
    s_color_mask = mask;
}

void LED_SetMode(LedMode_t mode)
{
    s_mode = mode;
}

void LED_SetBlinkParams(uint32_t on_ms, uint32_t off_ms, uint8_t count, uint32_t pause_ms)
{
    s_on_time_ms       = (on_ms  > 0) ? on_ms  : 1;
    s_off_time_ms      = (off_ms > 0) ? off_ms : 1;
    s_repeat_count     = count;
    s_pause_after_ms   = pause_ms;
}

void LED_SetState(uint8_t mask, LedMode_t mode, uint32_t on_ms, uint32_t off_ms, uint8_t count, uint32_t pause_ms)
{
    s_color_mask = mask;
    s_mode = mode;
    s_on_time_ms       = (on_ms  > 0) ? on_ms  : 1;
    s_off_time_ms      = (off_ms > 0) ? off_ms : 1;
    s_repeat_count     = count;
    s_pause_after_ms   = pause_ms;
}