//
// Created by Codex on 26-7-11.
//

#ifndef SPIN_MASTER_V2_ROTARY_ENCODER_H
#define SPIN_MASTER_V2_ROTARY_ENCODER_H

#include <stdbool.h>
#include <stdint.h>

typedef struct {
    int32_t position;
    bool button_pressed;
} RotaryEncoder;

extern RotaryEncoder rotary_encoder;

void RotaryEncoder_Init(void);
void RotaryEncoder_Update(void);
void RotaryEncoder_ResetPosition(void);
int32_t RotaryEncoder_GetPosition(void);
bool RotaryEncoder_IsPressed(void);

#endif //SPIN_MASTER_V2_ROTARY_ENCODER_H
