//
// Created by e on 26-1-20.
//

#ifndef SPIN_MASTER_V2_IDENTIFY_H
#define SPIN_MASTER_V2_IDENTIFY_H
#include "arm_math.h"

typedef struct MotorSystem MotorSystem;

typedef enum{
    RESISTANCE_IDENTIFICATION,  //电阻识别
    INDUCTANCE_IDENTIFICATION,  //电感识别
    IDENTIFY_DONE,
}Motor_IdentifyState;

typedef struct
{
    uint8_t    Flag;
    Motor_IdentifyState    state;
    uint8_t   EndFlag;           // 辨识完成标志
    uint16_t   Count;            // 计数
    uint16_t  WaitTim;           // 等待时间
    float Rs;                    // 相电阻
    float Ls;                    // 相电感
    float Lq;                    // q轴电感
    float Ld;                    // d轴电感
    float Flux;                  // 磁链
    float LsSum;                 // 电感累计值
    float CurMax;                // 最大识别电流
    float CurSum;                // 电流累计值
    float CurAverage[2];         // 电流平均值
    float VoltageSet[2];         // 电压给定值
}IDENTIFY_STRUCT;

void Motor_Identify(MotorSystem *p);

#endif //SPIN_MASTER_V2_IDENTIFY_H
