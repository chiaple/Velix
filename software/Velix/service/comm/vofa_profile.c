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

    VOFA_SendChannels(p->foc.iu,
                      p->foc.iv,
                      p->foc.iw,
                      p->sample.BusReal);
}
