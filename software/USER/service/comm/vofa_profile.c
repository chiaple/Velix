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

    switch (p->cmd.state) {
        case MOTOR_IDENTIFY:
            VOFA_SendChannels((float)p->identify.state,
                              p->identify.CurAverage[0],
                              p->identify.VoltageSet[0],
                              p->encoder.MechanicalSpeed);
            break;

        case MOTOR_SENSORLESS:
            if (p->cmd.mode == HFI_SMO_SPEED_CURRENT_LOOP) {
                VOFA_SendChannels(p->HfiSmoSwitch.HfiThetaRad,
                                  p->foc.iq,
                                  p->HfiSmoSwitch.OutputEleSpeedRpm / (float)p->motor.polePairs,
                                  p->foc.id);
            } else if ((p->cmd.mode == HFI_CURRENT_CLOSE) ||
                (p->cmd.mode == HFI_SPEED_CURRENT_CLOSE) ||
                (p->cmd.mode == HFI_POS_SPEED_CURRENT_CLOSE)) {
                VOFA_SendChannels(p->cmd.current_q,
                                  p->foc.iq,
                                  p->HFI.IdRef,
                                  p->encoder.MechanicalSpeed);
            } else {
                VOFA_SendChannels(p->cmd.speed,
                                  p->encoder.MechanicalSpeed,
                                  p->SPLL.ThetaCompensate,
                                  p->encoder.MechanicalSpeed);
            }
            break;

        case MOTOR_SENSORUSE:
            switch (p->cmd.mode) {
                case CONTROL_MODE_POSITION:
                    VOFA_SendChannels(p->cmd.position,
                                      p->encoder.absDegAngle,
                                      p->cmd.speed,
                                      p->encoder.MechanicalSpeed);
                    break;

                case CONTROL_MODE_CURRENT:
                    VOFA_SendChannels(p->foc.id,
                                      p->foc.iq,
                                      p->cmd.current_q,
                                      p->encoder.MechanicalSpeed);
                    break;

                case CONTROL_MODE_SPEED:
                default:
                    VOFA_SendChannels(p->cmd.speed,
                                      p->encoder.MechanicalSpeed,
                                      p->foc.iq,
                                      p->encoder.MechanicalSpeed);
                    break;
            }
            break;

        case MOTOR_STOP:
        default:
            VOFA_SendChannels(p->foc.iu,
                              p->foc.iv,
                              p->foc.iw,
                              p->encoder.MechanicalSpeed);
            break;
    }
}
