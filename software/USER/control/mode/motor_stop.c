#include "motor_stop.h"
#include "global.h"
#include "velocity.h"
#include "observer.h"
#include "command.h"
#include "PID.h"

void Motor_StopReset(MotorSystem *p)
{
    p->foc.Uq = 0;
    p->foc.Ud = 0;
    p->encoder.elecAngleRad = 0;
    p->encoder.HfiAbsDegAngle = 0.0f;
    p->encoder.HfiLastElecAngleRad = 0.0f;
    p->encoder.HfiPositionInit = 0U;

    p->encoder.velocity = 0;
    p->encoder.MechanicalSpeed = 0;
    p->encoder.RPM = 0;
    p->foc.IdLPF = 0;
    p->foc.IqLPF = 0;

    ResetVelocity(&p->encoder);
    ResetCmd(&p->cmd);

    // Reset observers and PLL to avoid stale internal states after stop
    ResetSMO(&p->SMO);
    ResetHFI(&p->HFI);
    ResetPLL(&p->SPLL);
    ResetPLL(&p->HPLL);

    // Reset key PID integrators to avoid residual outputs
    PID_reset(&p->iq_pid);
    PID_reset(&p->id_pid);
    PID_reset(&p->speed_pid);
    PID_reset(&p->HFI_pos_pid);
    PID_reset(&p->HFI_speed_pid);
    PID_reset(&p->HFI_id_pid);

    //重置参数识别
    p->identify.state = RESISTANCE_IDENTIFICATION;
    p->identify.Flag = 0;
    p->identify.EndFlag = 0;
    p->identify.Count = 0;
    p->identify.WaitTim = 0;
    p->identify.CurSum = 0.0f;
    p->identify.LsSum = 0.0f;

}
