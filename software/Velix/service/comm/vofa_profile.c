//
// Created by Codex on 26-4-6.
//

#include "vofa_profile.h"

#include "global.h"
#include "serial.h"

void VOFA_SendByMode(const MotorSystem *p)
{
    if (p == NULL) {
        return;
    }

    VOFA_SendChannels((float)p->sample.IuRaw,
                      (float)p->sample.IvRaw,
                      (float)p->sample.BusRaw,
                      (float)(p->sample.IvRaw - (int32_t)p->sample.IvOffset));
}
