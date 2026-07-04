//
// Created by 18135 on 26-1-17.
//

#ifndef SPIN_MASTER_V2_KEY_H
#define SPIN_MASTER_V2_KEY_H

#include <stdbool.h>
#include "config.h"

#define KEY_SCAN_PERIOD     10
#define KEY_LONG_TIME       500 / KEY_SCAN_PERIOD
#define KEY_DOUBLE_TIME     100 / KEY_SCAN_PERIOD

typedef enum {
    KEY_EVENT_NONE = 0,
    KEY_EVENT_SINGLE_CLICK,
    KEY_EVENT_DOUBLE_CLICK,
    KEY_EVENT_LONG_PRESS
}KeyEvent;

typedef struct {
    Velix_GpioPort* GPIOx;
    uint16_t GPIO_Pin;

    bool last_state;
    uint16_t press_cnt;
    uint16_t release_cnt;
    uint8_t click_count;
    bool long_flag;

    KeyEvent event;
}Key;

extern Key key_1,key_2;;

void Key_Init(Key* key, Velix_GpioPort* GPIOx, uint16_t GPIO_Pin);
void Key_Scan(Key* key);
KeyEvent key_GetEvent(Key* key);
void Key_Handler(Key* key,void(*single_cb)(void),void(*double_cb)(void),void(*long_cb)(void));

#endif //SPIN_MASTER_V2_KEY_H
