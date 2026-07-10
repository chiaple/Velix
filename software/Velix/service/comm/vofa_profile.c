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

    VOFA_SendChannels((float)p->foc.iu,
                      (float)p->foc.iv,
                      (float)p->foc.iw,
                      p->sample.BusReal);
}
