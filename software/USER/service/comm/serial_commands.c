#include <string.h>
#include <stdio.h>
#include "serial_commands.h"
#include "serial.h"
#include "global.h"
#include "tone_presets.h"

typedef struct {
    const char *name;
    uint8_t id;
    uint8_t exact_match;
} SerialCommandEntry;

enum {
    CMD_SENSORUSE = 1,
    CMD_SENSORLESS,
    CMD_IDENTIFY,
    CMD_SMO,
    CMD_HFI,
    CMD_HFISMO,
    CMD_SET_SPEED,
    CMD_SET_CUR,
    CMD_TONE1,
    CMD_TONE2,
    CMD_TONE3,
    CMD_TONE4,
    CMD_TONE5,
    CMD_SET_POS,
    CMD_STOP,
    CMD_GET_STATUS,
};

static const SerialCommandEntry serial_command_table[] = {
        {"SENSORUSE",  CMD_SENSORUSE,  1U},
        {"SENSORLESS", CMD_SENSORLESS, 1U},
        {"IDENTIFY",   CMD_IDENTIFY,   1U},
        {"SMO",        CMD_SMO,        1U},
        {"HFI",        CMD_HFI,        1U},
        {"HFISMO",     CMD_HFISMO,     1U},
        {"SET_SPEED ", CMD_SET_SPEED,  0U},
        {"SET_CUR ",   CMD_SET_CUR,    0U},
        {"TONE1",      CMD_TONE1,      1U},
        {"TONE2",      CMD_TONE2,      1U},
        {"TONE3",      CMD_TONE3,      1U},
        {"TONE4",      CMD_TONE4,      1U},
        {"TONE5",      CMD_TONE5,      1U},
        {"SET_POS ",   CMD_SET_POS,    0U},
        {"STOP",       CMD_STOP,       1U},
        {"GET_STATUS", CMD_GET_STATUS, 1U},
};

static const SerialCommandEntry *FindSerialCommand(const char *cmd)
{
    size_t i = 0;

    for (i = 0; i < (sizeof(serial_command_table) / sizeof(serial_command_table[0])); i++) {
        const SerialCommandEntry *entry = &serial_command_table[i];
        size_t name_len = strlen(entry->name);

        if (entry->exact_match != 0U) {
            if (strcmp(cmd, entry->name) == 0) {
                return entry;
            }
        } else if (strncmp(cmd, entry->name, name_len) == 0) {
            return entry;
        }
    }

    return NULL;
}

void process_single_command(uint32_t start_idx, uint32_t len)
{
    int value = 0;
    char cmd[128];
    const SerialCommandEntry *entry = NULL;

    if (len >= sizeof(cmd)) {
        len = sizeof(cmd) - 1U;
    }

    for (uint32_t i = 0; i < len; i++) {
        cmd[i] = (char)serial_dma_rx_buf[(start_idx + i) % RX_BUF_SIZE];
    }
    cmd[len] = '\0';

    if ((len >= 2U) && (cmd[len - 2U] == '\r') && (cmd[len - 1U] == '\n')) {
        cmd[len - 2U] = '\0';
    }

    entry = FindSerialCommand(cmd);
    if (entry == NULL) {
        return;
    }

    switch (entry->id) {
        case CMD_SENSORUSE:
            Mt.cmd.state = MOTOR_SENSORUSE;
            Mt.cmd.mode = ENCODER_CALIB;
            LED_SetState(LED_COLOR_GREEN, LED_MODE_CUSTOM_BLINK, 100, 100, 3, 900);
            break;
        case CMD_SENSORLESS:
            Mt.cmd.state = MOTOR_IDENTIFY;
            LED_SetState(LED_COLOR_CYAN, LED_MODE_CUSTOM_BLINK, 100, 100, 3, 900);
            break;
        case CMD_IDENTIFY:
            Mt.cmd.state = MOTOR_IDENTIFY;
            LED_SetState(LED_COLOR_YELLOW, LED_MODE_CUSTOM_BLINK, 100, 100, 3, 900);
            break;
        case CMD_SMO:
            Mt.cmd.mode = STRONG_DRAG_SMO_SPEED_CURRENT_LOOP;
            LED_SetState(LED_COLOR_WHITE, LED_MODE_CUSTOM_BLINK, 100, 100, 2, 900);
            break;
        case CMD_HFI:
            Mt.cmd.state = MOTOR_SENSORLESS;
            Mt.cmd.mode = HFI_CURRENT_CLOSE;
            LED_SetState(LED_COLOR_WHITE, LED_MODE_CUSTOM_BLINK, 100, 100, 4, 900);
            break;
        case CMD_HFISMO:
            Mt.cmd.state = MOTOR_SENSORLESS;
            Mt.cmd.mode = HFI_SMO_SPEED_CURRENT_LOOP;
            LED_SetState(LED_COLOR_CYAN, LED_MODE_CUSTOM_BLINK, 100, 100, 4, 900);
            break;
        case CMD_SET_SPEED:
            if (sscanf(cmd + strlen("SET_SPEED "), "%d", &value) == 1) {
                Mt.cmd.fSpeed = (float)value;
                LED_SetState(LED_COLOR_BLUE, LED_MODE_CUSTOM_BLINK, 100, 100, 2, 900);
            }
            break;
        case CMD_SET_CUR:
            if (sscanf(cmd + strlen("SET_CUR "), "%d", &value) == 1) {
                Mt.cmd.state = MOTOR_SENSORUSE;
                Mt.cmd.current_q = (float)value;
                Mt.cmd.mode = CONTROL_MODE_CURRENT;
                LED_SetState(LED_COLOR_RED, LED_MODE_CUSTOM_BLINK, 100, 100, 2, 900);
            }
            break;
        case CMD_TONE1:
            MotorTone_PlayPreset(1);
            break;
        case CMD_TONE2:
            MotorTone_PlayPreset(2);
            break;
        case CMD_TONE3:
            MotorTone_PlayPreset(3);
            break;
        case CMD_TONE4:
            MotorTone_PlayPreset(4);
            break;
        case CMD_TONE5:
            MotorTone_PlayPreset(5);
            break;
        case CMD_SET_POS:
            if (sscanf(cmd + strlen("SET_POS "), "%d", &value) == 1) {
                //Mt.cmd.state = MOTOR_SENSORUSE;
                // if (Mt.cmd.mode != CONTROL_MODE_POSITION) {
                //     Mt.cmd.position = Mt.encoder.absDegAngle;
                //     Mt.cmd.speed = 0.0f;
                //     Mt.cmd.fSpeed = 0.0f;
                // }
                Mt.cmd.fPosition = (float)value;
                //Mt.cmd.mode = CONTROL_MODE_POSITION;
                LED_SetState(LED_COLOR_MAGENTA, LED_MODE_CUSTOM_BLINK, 100, 100, 3, 900);
            }
            break;
        case CMD_STOP:
            Mt.cmd.mode = CONTROL_MODE_IDLE;
            Mt.cmd.state = MOTOR_STOP;
            LED_SetState(LED_COLOR_RED, LED_MODE_CUSTOM_BLINK, 100, 100, 4, 900);
            break;
        case CMD_GET_STATUS:
        default:
            break;
    }
}
