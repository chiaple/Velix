//
// Created by e on 26-1-18.
//
#include "global.h"
#include "sensoruse.h"
#include "FOC.h"
#include "motor_tone.h"
#include "velocity.h"
#include "command.h"
#include "PID.h"

static uint8_t calib_started = 0U;
static uint32_t calib_start_ms = 0U;

 void Sensoruse_EncoderCalib(MotorSystem *p)
{
    if (calib_started == 0U) {
        calib_started = 1U;
        calib_start_ms = sys_tick_ms;

        MotorTone_Stop();
        ResetCmd(&p->cmd);
        ResetVelocity(&p->encoder);

        p->encoder.numOfCircles = 0;
        p->encoder.absDegAngle = 0.0f;
        p->encoder.curDegAngle = 0.0f;
        p->encoder.degAngle = 0.0f;
        p->encoder.angle = 0.0f;
        p->encoder.elecAngleRad = 0.0f;
        p->encoder.PosCalculateCnt = 0U;

        p->foc.Uq = 0.0f;
        p->foc.Ud = 0.0f;
        p->foc.IdLPF = 0.0f;
        p->foc.IqLPF = 0.0f;

        PID_reset(&p->pos_pid);
        PID_reset(&p->speed_pid);
        PID_reset(&p->speed_low_pid);
        PID_reset(&p->iq_pid);
        PID_reset(&p->id_pid);
    }

    p->encoder.elecAngleRad = 0.0f;
    p->foc.Uq = 0.0f;
    p->foc.Ud = ENCODER_CALIB_UD_DEFAULT;

    if ((sys_tick_ms - calib_start_ms) >= ENCODER_CALIB_TIME_MS_DEFAULT) {
        ReadAngleDeg(&p->encoder);
        p->encoder.zeroEAngle = p->encoder.encoderRaw;

        p->foc.Uq = 0.0f;
        p->foc.Ud = 0.0f;
        p->encoder.elecAngleRad = 0.0f;
        p->encoder.velocity = 0.0f;
        p->encoder.RPM = 0.0f;
        p->encoder.MechanicalSpeed = 0.0f;
        p->encoder.lastAngle = 0.0f;
        p->encoder.lastTs = sys_tick_ms;

        p->cmd.mode = CONTROL_MODE_POSITION;
        calib_started = 0U;
    }
}

void CurLoop(MotorSystem *p){

    Clarke_Transform(p->foc.iu, p->foc.iv, p->foc.iw, &p->foc.i_alpha, &p->foc.i_beta);
    Park_Transform(p->foc.i_alpha, p->foc.i_beta, p->encoder.elecAngleRad, &p->foc.id, &p->foc.iq);

    p->foc.IqLPF = p->foc.iq * p->foc.IqLPFFactor + p->foc.IqLPF * (1-p->foc.IqLPFFactor);
    p->foc.IdLPF = p->foc.id * p->foc.IdLPFFactor + p->foc.IdLPF * (1-p->foc.IdLPFFactor);

    p->foc.Uq = PID(&p->iq_pid, p->cmd.current_q,p->foc.IqLPF);
    p->foc.Ud = PID(&p->id_pid,0,p->foc.IdLPF);

}



void VelLoop(MotorSystem *p){
    p->velocity.SpeedCalculateCnt++;
    if(p->velocity.SpeedCalculateCnt >= 2)
    {
        p->velocity.SpeedCalculateCnt = 0;
        CalcSpeed(&p->encoder);
        p->encoder.MechanicalSpeed = p->encoder.RPM;
        if(p->cmd.mode == CONTROL_MODE_SPEED)
            p->cmd.current_q = PID(&p->speed_pid, p->cmd.speed, p->encoder.RPM);
        else if(p->cmd.mode == CONTROL_MODE_POSITION)
            p->cmd.current_q = PID(&p->speed_low_pid, p->cmd.speed, p->encoder.RPM);
    }

    CurLoop(p);
}

void position(MotorSystem *p){
    p->encoder.PosCalculateCnt++;
    if(p->encoder.PosCalculateCnt>=4){
        p->encoder.PosCalculateCnt = 0;
        p->cmd.speed = PID(&p->pos_pid, p->cmd.position, p->encoder.absDegAngle);
    }
    VelLoop(p);
}


void Sensoruse_Control(MotorSystem *p){
    ReadAngleDeg(&p->encoder);

    if (p->cmd.mode == ENCODER_CALIB)
    {
        Sensoruse_EncoderCalib(p);
        return;
    }

    calib_started = 0U;

    Get_position(&p->encoder);
    p->encoder.elecAngleRad = GetElectric_Angle(&p->encoder, &p->motor);//获取电角度

    switch (p->cmd.mode)
    {
        case CONTROL_MODE_POSITION:
        {
            position(p);
        }
            break;
        case CONTROL_MODE_SPEED:
        {
            VelLoop(p);
        }
            break;
        case CONTROL_MODE_CURRENT:
        {
            CurLoop(p);
        }
            break;
        case CONTROL_MODE_IDLE:
        {
            p->encoder.elecAngleRad = 0;
            p->foc.Uq = 0;
            p->foc.Ud = 0;
        }
            break;
    }
}
