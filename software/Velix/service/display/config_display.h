//
// Created by Codex on 26-7-11.
//

#ifndef SPIN_MASTER_V2_CONFIG_DISPLAY_H
#define SPIN_MASTER_V2_CONFIG_DISPLAY_H

#include "global.h"
#include "key.h"

void ConfigDisplay_Init(void);
void ConfigDisplay_Task(MotorSystem *motor);
void ConfigDisplay_HandleKey(MotorSystem *motor, KeyEvent key1_event, KeyEvent key2_event);

#endif // SPIN_MASTER_V2_CONFIG_DISPLAY_H
