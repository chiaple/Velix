//
// Created by Codex on 26-7-11.
//

#include "rotary_encoder.h"

#include "config.h"

RotaryEncoder rotary_encoder;

void RotaryEncoder_Init(void)
{
    rotary_encoder.position = 0;
    rotary_encoder.button_pressed = false;

    Velix_RotarySetCounter(0);
    (void)Velix_RotaryStart();
}

void RotaryEncoder_Update(void)
{
    static bool last_button_pressed = false;
    const int16_t delta = (int16_t)Velix_RotaryGetCounter() * ROTARY_DIRECTION_DEFAULT;
    const bool button_pressed =
        (VELIX_GPIO_IS_SET(VELIX_ROTARY_BUTTON_GPIO_Port, VELIX_ROTARY_BUTTON_Pin) == 0U);

    Velix_RotarySetCounter(0);

    rotary_encoder.position += delta;
    rotary_encoder.button_pressed = button_pressed;

    if (button_pressed && !last_button_pressed) {
        RotaryEncoder_ResetPosition();
    }

    last_button_pressed = button_pressed;
}

void RotaryEncoder_ResetPosition(void)
{
    rotary_encoder.position = 0;
    Velix_RotarySetCounter(0);
}

int32_t RotaryEncoder_GetPosition(void)
{
    return rotary_encoder.position;
}

bool RotaryEncoder_IsPressed(void)
{
    return rotary_encoder.button_pressed;
}
