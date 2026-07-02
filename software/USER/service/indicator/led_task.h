// led_task.h
#ifndef SPIN_MASTER_V2_LED_TASK_H
#define SPIN_MASTER_V2_LED_TASK_H

#include "main.h"


// ────────────────────────────────────────────────
//  GPIO 操作宏（你的共阳极定义：低电平点亮）
// ────────────────────────────────────────────────
#define LED_ON(port, pin)   HAL_GPIO_WritePin(port, pin, GPIO_PIN_RESET)
#define LED_OFF(port, pin)  HAL_GPIO_WritePin(port, pin, GPIO_PIN_SET)

// ────────────────────────────────────────────────
//  颜色掩码（位定义）
// ────────────────────────────────────────────────
#define LED_COLOR_R         (1U << 0)
#define LED_COLOR_G         (1U << 1)
#define LED_COLOR_B         (1U << 2)

#define LED_COLOR_OFF       (0)
#define LED_COLOR_RED       (LED_COLOR_R)
#define LED_COLOR_GREEN     (LED_COLOR_G)
#define LED_COLOR_BLUE      (LED_COLOR_B)
#define LED_COLOR_YELLOW    (LED_COLOR_R | LED_COLOR_G)
#define LED_COLOR_CYAN      (LED_COLOR_G | LED_COLOR_B)
#define LED_COLOR_MAGENTA   (LED_COLOR_R | LED_COLOR_B)
#define LED_COLOR_WHITE     (LED_COLOR_R | LED_COLOR_G | LED_COLOR_B)

// ────────────────────────────────────────────────
//  闪烁模式（简化后只剩三种核心模式）
// ────────────────────────────────────────────────
typedef enum {
    LED_MODE_OFF = 0,           // 全灭
    LED_MODE_SOLID,             // 常亮
    LED_MODE_CUSTOM_BLINK,      // 自定义闪烁（最灵活）
} LedMode_t;

// GPIO 端口/引脚（从 CubeMX 生成的 main.h 引用）
#define LED_R_PORT      LED_R_GPIO_Port
#define LED_R_PIN       LED_R_Pin
#define LED_G_PORT      LED_G_GPIO_Port
#define LED_G_PIN       LED_G_Pin
#define LED_B_PORT      LED_B_GPIO_Port
#define LED_B_PIN       LED_B_Pin

// ────────────────────────────────────────────────
//  API 函数声明
// ────────────────────────────────────────────────
void LED_Init(void);
void LED_Tick_1ms(void);

void LED_SetColorMask(uint8_t mask);
void LED_SetMode(LedMode_t mode);

// 自定义闪烁参数（单位：毫秒）
void LED_SetBlinkParams(uint32_t on_time_ms,      // 每次亮多久
                        uint32_t off_time_ms,     // 每次灭多久
                        uint8_t  repeat_count,    // 闪几次（0 = 无限循环）
                        uint32_t pause_after_ms); // 闪完 N 次后暂停多久再重复（0 = 不暂停）
void LED_SetState(uint8_t mask, LedMode_t mode, uint32_t on_ms, uint32_t off_ms, uint8_t count, uint32_t pause_ms);



#endif // SPIN_MASTER_V2_LED_TASK_H