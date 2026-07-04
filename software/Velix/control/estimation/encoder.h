//
// Created by e on 26-1-22.
//

#ifndef SPIN_MASTER_V2_ENCODER_H
#define SPIN_MASTER_V2_ENCODER_H
#include "arm_math.h"
typedef struct {

    int         Dir;
    float32_t     EncoderValMax;        // 编码器最大原始值
    uint16_t zeroEAngle;              //编码器校准

    uint16_t    encoderRaw;           //编码器原始数据
    float32_t	degAngle;			// 角度：0-360
    float32_t 	angle;				// 角度：0-2pi
    float32_t   curDegAngle; 		// 当前单圈角度（0-360°）
    int32_t  	numOfCircles;		// 累计圈数（正数为顺时针，负数为逆时针）
    float32_t   absDegAngle;		// 绝对角度 = curDegAngle + numOfCircles*360

    float32_t    elecAngleRad;        //电角度
    float32_t    HfiAbsDegAngle;      // HFI位置环使用的累计机械角度
    float32_t    HfiLastElecAngleRad; // HFI上一周期电角度
    uint8_t      HfiPositionInit;     // HFI累计角度初始化标志

    float ElectricalValSet;           // 给定的电角度
    float ElectricalSpdSet;           // 给定的电角速度

    float lsjd; //零时角度
    float djd;//临时电角度



    uint16_t PosCalculateCnt;

    // 速度
    float32_t 	velocity;
    float32_t   RPM;
    uint32_t 	lastTs;				// 上一个时间
    float32_t 	lastAngle;			// 上一个角度

    float ElectricalSpeedRaw;         // 原始电角速度
    float ElectricalSpeedLPF;         // 原始电角速度滤波值
    float ElectricalSpeedLPFFactor;

    float MechanicalSpeed;            //机械速度
    float MechanicalSpeedSet;         // 目标机械速度
    float rawVel;

    float32_t angletest;

}ENCODER_STRUCT;

void Electrical_Angle_Generator(ENCODER_STRUCT *p);

void ReadAngleDeg(ENCODER_STRUCT *p);
void Get_position(ENCODER_STRUCT *p);

#endif //SPIN_MASTER_V2_ENCODER_H
