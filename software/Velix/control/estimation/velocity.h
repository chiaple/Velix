//
// Created by e on 26-1-17.
//

#ifndef SPIN_MASTER_V2_VELOCITY_H
#define SPIN_MASTER_V2_VELOCITY_H

#include "MT6701.h"
#include "encoder.h"

typedef struct {
    uint16_t  SpeedCalculateCnt;    //速度计数

    float MechanicalSpeedSet;         // 给定的机械速度
    float MechanicalSpeedSetLast;     // 上次目标机械速度

    int32_t   ElectricalPosThis;          // 本次电角位置

}VELOCITY_STRUCT;




void CalcSpeed(ENCODER_STRUCT *p);
void ResetVelocity(ENCODER_STRUCT *p);
#endif //SPIN_MASTER_V2_VELOCITY_H
