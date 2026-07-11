//
// Created by Codex on 26-7-11.
//

#include "config_display.h"

#include <stdio.h>
#include <stdint.h>
#include "led_task.h"
#include "rotary_encoder.h"
#include "st7789.h"

#define MENU_TOP_Y      44U
#define MENU_ROW_H      24U
#define MENU_VISIBLE_ROWS 5U
#define MENU_AREA_Y0    (MENU_TOP_Y - 4U)
#define MENU_AREA_Y1    (MENU_TOP_Y + (MENU_ROW_H * MENU_VISIBLE_ROWS) + 2U)
#define MENU_HILITE_X0  8U
#define MENU_HILITE_X1  (ST7789_WIDTH - 8U)

static uint8_t s_screen_drawn = 0U;
static uint8_t s_run_page_drawn = 0U;
static uint8_t s_identify_page_drawn = 0U;
static uint8_t s_wait_sensorless_identify = 0U;
static uint32_t s_last_refresh_ms = 0U;
static int8_t s_last_menu_index = -1;
static int32_t s_scroll_offset = 0;

typedef enum {
    MENU_PAGE_MAIN = 0,
    MENU_PAGE_SENSORLESS,
} ConfigMenuPage;

typedef enum {
    MENU_ACTION_RUN = 0,
    MENU_ACTION_SUBMENU,
    MENU_ACTION_BACK,
    MENU_ACTION_SENSORLESS_IDENTIFY,
} ConfigMenuAction;

typedef struct {
    const char *title;
    Motor_ControlState state;
    Motor_ControlMode mode;
    uint8_t reset_speed;
    ConfigMenuAction action;
    ConfigMenuPage next_page;
} ConfigMenuItem;

static ConfigMenuPage s_menu_page = MENU_PAGE_MAIN;

static const ConfigMenuItem kMainMenuItems[] = {
    {"SENSORUSE",   MOTOR_SENSORUSE,  ENCODER_CALIB, 1U, MENU_ACTION_RUN,     MENU_PAGE_MAIN},
    {"SENSORLESS",  MOTOR_IDENTIFY,   CONTROL_MODE_IDLE, 1U, MENU_ACTION_SENSORLESS_IDENTIFY, MENU_PAGE_MAIN},
    {"SLESS MODE >", MOTOR_STOP,      CONTROL_MODE_IDLE, 0U, MENU_ACTION_SUBMENU, MENU_PAGE_SENSORLESS},
    {"IDENTIFY",    MOTOR_IDENTIFY,   CONTROL_MODE_IDLE, 1U, MENU_ACTION_RUN,     MENU_PAGE_MAIN},
    {"STOP",        MOTOR_STOP,       CONTROL_MODE_IDLE, 1U, MENU_ACTION_RUN,     MENU_PAGE_MAIN},
};

static const ConfigMenuItem kSensorlessMenuItems[] = {
    {"IDENTIFY", MOTOR_IDENTIFY,   CONTROL_MODE_IDLE, 1U, MENU_ACTION_RUN,  MENU_PAGE_SENSORLESS},
    {"SMO",      MOTOR_SENSORLESS, STRONG_DRAG_SMO_SPEED_CURRENT_LOOP, 1U, MENU_ACTION_RUN, MENU_PAGE_SENSORLESS},
    {"HFI",      MOTOR_SENSORLESS, HFI_CURRENT_CLOSE, 1U, MENU_ACTION_RUN, MENU_PAGE_SENSORLESS},
    {"HFISMO",   MOTOR_SENSORLESS, HFI_SMO_SPEED_CURRENT_LOOP, 1U, MENU_ACTION_RUN, MENU_PAGE_SENSORLESS},
    {"< BACK",   MOTOR_STOP,       CONTROL_MODE_IDLE, 0U, MENU_ACTION_BACK, MENU_PAGE_MAIN},
};

static const ConfigMenuItem *GetActiveMenuItems(void)
{
    return (s_menu_page == MENU_PAGE_SENSORLESS) ? kSensorlessMenuItems : kMainMenuItems;
}

static int32_t GetActiveMenuCount(void)
{
    if (s_menu_page == MENU_PAGE_SENSORLESS) {
        return (int32_t)(sizeof(kSensorlessMenuItems) / sizeof(kSensorlessMenuItems[0]));
    }
    return (int32_t)(sizeof(kMainMenuItems) / sizeof(kMainMenuItems[0]));
}

static int32_t WrapIndex(int32_t index, int32_t count)
{
    if (count <= 0) {
        return 0;
    }
    index %= count;
    if (index < 0) {
        index += count;
    }
    return index;
}

static int32_t GetMenuIndex(void)
{
    int32_t step = CONFIG_MENU_COUNTS_PER_STEP_DEFAULT;
    if (step <= 0) {
        step = 1;
    }
    return WrapIndex(RotaryEncoder_GetPosition() / step, GetActiveMenuCount());
}

static void EnterMenuPage(ConfigMenuPage page)
{
    s_menu_page = page;
    s_screen_drawn = 0U;
    s_run_page_drawn = 0U;
    s_identify_page_drawn = 0U;
    s_last_menu_index = -1;
    s_scroll_offset = 0;
    RotaryEncoder_ResetPosition();
}

static void StartSensorlessIdentify(MotorSystem *motor)
{
    motor->identify.state = RESISTANCE_IDENTIFICATION;
    motor->identify.Flag = 0U;
    motor->identify.EndFlag = 0U;
    motor->identify.Count = 0U;
    motor->identify.WaitTim = 0U;
    motor->identify.CurSum = 0.0f;
    motor->identify.LsSum = 0.0f;

    motor->cmd.state = MOTOR_IDENTIFY;
    motor->cmd.mode = CONTROL_MODE_IDLE;
    motor->cmd.speed = 0.0f;
    motor->cmd.fSpeed = 0.0f;
    RotaryEncoder_ResetPosition();

    s_wait_sensorless_identify = 1U;
    s_screen_drawn = 0U;
    s_run_page_drawn = 0U;
    s_identify_page_drawn = 0U;
    s_last_refresh_ms = 0U;

    LED_SetState(LED_COLOR_CYAN, LED_MODE_CUSTOM_BLINK, 100, 100, 3, 900);
}

static void ApplyMenuCommand(MotorSystem *motor, int32_t menu_index)
{
    const ConfigMenuItem *item = &GetActiveMenuItems()[menu_index];

    if (item->action == MENU_ACTION_SUBMENU) {
        EnterMenuPage(item->next_page);
        return;
    }

    if (item->action == MENU_ACTION_BACK) {
        EnterMenuPage(MENU_PAGE_MAIN);
        return;
    }

    if (item->action == MENU_ACTION_SENSORLESS_IDENTIFY) {
        StartSensorlessIdentify(motor);
        return;
    }

    motor->cmd.state = item->state;
    motor->cmd.mode = item->mode;

    if (item->reset_speed != 0U) {
        motor->cmd.speed = 0.0f;
        motor->cmd.fSpeed = 0.0f;
        RotaryEncoder_ResetPosition();
    }

    switch (item->state) {
        case MOTOR_SENSORUSE:
            LED_SetState(LED_COLOR_GREEN, LED_MODE_CUSTOM_BLINK, 100, 100, 3, 900);
            break;
        case MOTOR_IDENTIFY:
            LED_SetState(LED_COLOR_YELLOW, LED_MODE_CUSTOM_BLINK, 100, 100, 3, 900);
            break;
        case MOTOR_SENSORLESS:
            LED_SetState(LED_COLOR_CYAN, LED_MODE_CUSTOM_BLINK, 100, 100, 3, 900);
            break;
        case MOTOR_STOP:
        default:
            LED_SetState(LED_COLOR_RED, LED_MODE_CUSTOM_BLINK, 100, 100, 4, 900);
            break;
    }
}

static void StopMotorFromMenu(MotorSystem *motor)
{
    motor->cmd.mode = CONTROL_MODE_IDLE;
    motor->cmd.state = MOTOR_STOP;
    motor->cmd.speed = 0.0f;
    motor->cmd.fSpeed = 0.0f;
    motor->cmd.current_q = 0.0f;
    motor->cmd.current_d = 0.0f;
    RotaryEncoder_ResetPosition();
    LED_SetState(LED_COLOR_RED, LED_MODE_CUSTOM_BLINK, 100, 100, 4, 900);

    s_wait_sensorless_identify = 0U;
    s_screen_drawn = 0U;
    s_run_page_drawn = 0U;
    s_identify_page_drawn = 0U;
    s_last_menu_index = -1;
    s_scroll_offset = 0;
    s_menu_page = MENU_PAGE_MAIN;
}

static void UpdateScrollOffset(int32_t selected_index)
{
    const int32_t visible_rows = (int32_t)MENU_VISIBLE_ROWS;
    const int32_t count = GetActiveMenuCount();

    if (selected_index < s_scroll_offset) {
        s_scroll_offset = selected_index;
    } else if (selected_index >= (s_scroll_offset + visible_rows)) {
        s_scroll_offset = selected_index - visible_rows + 1;
    }

    if (s_scroll_offset < 0) {
        s_scroll_offset = 0;
    }
    if (s_scroll_offset > (count - visible_rows)) {
        s_scroll_offset = (count > visible_rows) ? (count - visible_rows) : 0;
    }
}

static void DrawMenuRow(uint8_t row, int32_t item_index, uint8_t selected)
{
    const uint16_t y = MENU_TOP_Y + ((uint16_t)row * MENU_ROW_H);
    const uint16_t bg = selected ? BLUE : BLACK;
    const uint16_t fg = selected ? WHITE : LGRAY;
    const uint16_t accent = selected ? YELLOW : GRAY;
    const ConfigMenuItem *items = GetActiveMenuItems();

    ST7789_Fill(MENU_HILITE_X0, y - 2U, MENU_HILITE_X1, y + MENU_ROW_H - 4U, bg);
    ST7789_WriteString(14, y, selected ? ">" : " ", Font_11x18, accent, bg);
    ST7789_WriteString(34, y, items[item_index].title, Font_11x18, fg, bg);
}

static void DrawStaticPage(void)
{
    ST7789_Fill_Color(BLACK);
    ST7789_WriteString(10, 8,
                       (s_menu_page == MENU_PAGE_SENSORLESS) ? "Sensorless" : "Velix Menu",
                       Font_16x26, GREEN, BLACK);
    ST7789_DrawLine(10, 38, ST7789_WIDTH - 10, 38, GRAY);

    UpdateScrollOffset(GetMenuIndex());
    ST7789_Fill(MENU_HILITE_X0, MENU_AREA_Y0, MENU_HILITE_X1, MENU_AREA_Y1, BLACK);

    const int32_t count = GetActiveMenuCount();
    const int32_t selected = GetMenuIndex();
    for (uint8_t row = 0; row < MENU_VISIBLE_ROWS; row++) {
        int32_t item_index = s_scroll_offset + (int32_t)row;
        if (item_index >= count) {
            break;
        }
        DrawMenuRow(row, item_index, item_index == selected);
    }

    ST7789_DrawLine(10, 164, ST7789_WIDTH - 10, 164, GRAY);
}

static void FormatFixed(char *text, size_t text_size, float value, uint16_t decimals, const char *unit)
{
    const uint32_t scale = (decimals == 2U) ? 100U :
                           (decimals == 1U) ? 10U : 1U;
    int32_t scaled = (int32_t)((value * (float)scale) + ((value >= 0.0f) ? 0.5f : -0.5f));
    const char sign = (scaled < 0) ? '-' : ' ';

    if (scaled < 0) {
        scaled = -scaled;
    }

    const int32_t integer = scaled / (int32_t)scale;
    const int32_t fraction = scaled % (int32_t)scale;

    if (decimals == 2U) {
        snprintf(text, text_size, "%c%ld.%02ld %s", sign, (long)integer, (long)fraction, unit);
    } else if (decimals == 1U) {
        snprintf(text, text_size, "%c%ld.%01ld %s", sign, (long)integer, (long)fraction, unit);
    } else {
        snprintf(text, text_size, "%c%ld %s", sign, (long)integer, unit);
    }
}

static void DrawRunLabel(uint16_t y, const char *label)
{
    ST7789_WriteString(12, y, label, Font_11x18, CYAN, BLACK);
}

static void DrawRunValue(uint16_t y, const char *value)
{
    ST7789_Fill(115, y - 2U, ST7789_WIDTH - 8U, y + 18U, BLACK);
    ST7789_WriteString(115, y, value, Font_11x18, WHITE, BLACK);
}

static void DrawRunPageStatic(void)
{
    ST7789_Fill_Color(BLACK);
    ST7789_WriteString(10, 8, "Velix Run", Font_16x26, GREEN, BLACK);
    ST7789_DrawLine(10, 38, ST7789_WIDTH - 10, 38, GRAY);

    DrawRunLabel(50, "TARGET");
    DrawRunLabel(74, "SPEED");
    DrawRunLabel(98, "VBUS");
    DrawRunLabel(122, "IU");
    DrawRunLabel(146, "IV");
}

static void DrawRunPageValues(const MotorSystem *motor)
{
    char text[28];

    snprintf(text, sizeof(text), "%ld RPM", (long)motor->cmd.fSpeed);
    DrawRunValue(50, text);

    snprintf(text, sizeof(text), "%ld RPM", (long)motor->cmd.speed);
    DrawRunValue(74, text);

    FormatFixed(text, sizeof(text), motor->sample.BusReal, 1U, "V");
    DrawRunValue(98, text);

    FormatFixed(text, sizeof(text), motor->foc.iu, 2U, "A");
    DrawRunValue(122, text);

    FormatFixed(text, sizeof(text), motor->foc.iv, 2U, "A");
    DrawRunValue(146, text);
}

static void DrawIdentifyPageStatic(void)
{
    ST7789_Fill_Color(BLACK);
    ST7789_WriteString(10, 8, "Sensorless", Font_16x26, GREEN, BLACK);
    ST7789_DrawLine(10, 38, ST7789_WIDTH - 10, 38, GRAY);

    ST7789_WriteString(34, 70, "Identifying...", Font_16x26, YELLOW, BLACK);
    ST7789_WriteString(16, 142, "KEY2: Cancel", Font_11x18, LGRAY, BLACK);
}

static void DrawIdentifyPageValues(const MotorSystem *motor)
{
    const char *stage = "Resistance";

    if (motor->identify.state == INDUCTANCE_IDENTIFICATION) {
        stage = "Inductance";
    } else if (motor->identify.state == IDENTIFY_DONE) {
        stage = "Done";
    }

    ST7789_Fill(34, 108, ST7789_WIDTH - 16U, 130, BLACK);
    ST7789_WriteString(34, 108, stage, Font_11x18, CYAN, BLACK);
}

void ConfigDisplay_Init(void)
{
    s_screen_drawn = 0U;
    s_run_page_drawn = 0U;
    s_identify_page_drawn = 0U;
    s_wait_sensorless_identify = 0U;
    s_last_refresh_ms = 0U;
    s_last_menu_index = -1;
    s_scroll_offset = 0;
    s_menu_page = MENU_PAGE_MAIN;
}

void ConfigDisplay_Task(MotorSystem *motor)
{
    if (motor == NULL) {
        return;
    }

    if (s_wait_sensorless_identify != 0U) {
        if (motor->identify.EndFlag != 0U) {
            motor->cmd.state = MOTOR_STOP;
            motor->cmd.mode = CONTROL_MODE_IDLE;
            motor->cmd.speed = 0.0f;
            motor->cmd.fSpeed = 0.0f;
            motor->cmd.current_q = 0.0f;
            motor->cmd.current_d = 0.0f;
            s_wait_sensorless_identify = 0U;
            EnterMenuPage(MENU_PAGE_SENSORLESS);
            return;
        }

        s_screen_drawn = 0U;
        s_run_page_drawn = 0U;
        if (s_identify_page_drawn == 0U) {
            DrawIdentifyPageStatic();
            s_identify_page_drawn = 1U;
            s_last_refresh_ms = 0U;
        }

        if ((sys_tick_ms - s_last_refresh_ms) < CONFIG_DISPLAY_REFRESH_MS_DEFAULT) {
            return;
        }
        s_last_refresh_ms = sys_tick_ms;
        DrawIdentifyPageValues(motor);
        return;
    }

    if (motor->cmd.state != MOTOR_STOP) {
        s_screen_drawn = 0U;
        s_identify_page_drawn = 0U;
        s_last_menu_index = -1;
        s_scroll_offset = 0;
        if (s_run_page_drawn == 0U) {
            DrawRunPageStatic();
            s_run_page_drawn = 1U;
            s_last_refresh_ms = 0U;
        }

        if ((sys_tick_ms - s_last_refresh_ms) < CONFIG_DISPLAY_REFRESH_MS_DEFAULT) {
            return;
        }
        s_last_refresh_ms = sys_tick_ms;
        DrawRunPageValues(motor);
        return;
    }

    s_run_page_drawn = 0U;
    s_identify_page_drawn = 0U;

    if (s_screen_drawn == 0U) {
        DrawStaticPage();
        s_screen_drawn = 1U;
        s_last_refresh_ms = 0U;
        s_last_menu_index = -1;
    }

    if ((sys_tick_ms - s_last_refresh_ms) < CONFIG_DISPLAY_REFRESH_MS_DEFAULT) {
        return;
    }
    s_last_refresh_ms = sys_tick_ms;

    const int32_t menu_index = GetMenuIndex();
    if (menu_index != s_last_menu_index) {
        int32_t old_scroll = s_scroll_offset;
        UpdateScrollOffset(menu_index);

        if (old_scroll != s_scroll_offset || s_last_menu_index < 0) {
            ST7789_Fill(MENU_HILITE_X0, MENU_AREA_Y0, MENU_HILITE_X1, MENU_AREA_Y1, BLACK);

            const int32_t count = GetActiveMenuCount();
            for (uint8_t row = 0; row < MENU_VISIBLE_ROWS; row++) {
                int32_t item_index = s_scroll_offset + (int32_t)row;
                if (item_index >= count) {
                    break;
                }
                DrawMenuRow(row, item_index, item_index == menu_index);
            }
        } else {
            if (s_last_menu_index >= 0) {
                DrawMenuRow((uint8_t)(s_last_menu_index - s_scroll_offset),
                            s_last_menu_index,
                            0U);
            }
            DrawMenuRow((uint8_t)(menu_index - s_scroll_offset), menu_index, 1U);
        }
        s_last_menu_index = (int8_t)menu_index;
    }
}

void ConfigDisplay_HandleKey(MotorSystem *motor, KeyEvent key1_event, KeyEvent key2_event)
{
    if (motor == NULL) {
        return;
    }

    if (key2_event != KEY_EVENT_NONE) {
        s_wait_sensorless_identify = 0U;
        if (motor->cmd.state == MOTOR_STOP && s_menu_page != MENU_PAGE_MAIN) {
            EnterMenuPage(MENU_PAGE_MAIN);
            return;
        }
        StopMotorFromMenu(motor);
        return;
    }

    if (key1_event != KEY_EVENT_SINGLE_CLICK) {
        return;
    }

    if (motor->cmd.state == MOTOR_STOP) {
        ApplyMenuCommand(motor, GetMenuIndex());
    }
}
