//
// Created by Codex on 26-4-6.
//

#include "vofa_profile.h"

#include "rotary_encoder.h"
#include "global.h"
#include "serial.h"

void VOFA_SendByMode(const MotorSystem *p)
{
    if (p == NULL) {
        return;
    }

    VOFA_SendChannels((float)rotary_encoder.position,
                      p->cmd.fSpeed,
                      rotary_encoder.button_pressed ? 1.0f : 0.0f,
                      p->sample.BusReal);
}
