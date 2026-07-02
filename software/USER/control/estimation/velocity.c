//
// Created by e on 26-1-17.
//

#include "velocity.h"


#define TIM2_FREQUENCY 1000000 // 170MHz / PSC = 170000000 / 170 = 1MHz

void CalcSpeed(ENCODER_STRUCT *p){
   // uint32_t currentCount = TIM2 -> CNT;

    // 计算时间间隔（秒）
    float Ts = 0.0001f;
    /*if (currentCount >= p->lastTs) {
        Ts = (float)(currentCount - p->lastTs) / TIM2_FREQUENCY;
    } else { // 溢出
        Ts = (float)(currentCount + (4294967295 - p->lastTs) + 1) / TIM2_FREQUENCY;
    }*/

    // 计算角度变化
    float delta = p->angle - p->lastAngle;

    if (delta > M_PI) delta -= 2.0f * M_PI;
    if (delta < -M_PI) delta += 2.0f * M_PI;

    p->rawVel = delta / Ts;
    //低通滤波
    const float alpha = 0.01f;
    p->velocity = p->velocity * (1.0f - alpha) + p->rawVel * alpha;
    p->RPM = p->velocity * 60.0f / (2.0f * M_PI);
    p->lastAngle = p->angle;
    //p->lastTs = currentCount;  // 保存当前计数值
}

void ResetVelocity(ENCODER_STRUCT *p)
{
    p->velocity = 0.0f;
    p->RPM = 0.0f;
    p->lastAngle = 0.0f;
    p->angle = 0.0f;
    p->MechanicalSpeed = 0.0f;
    p->rawVel = 0.0f;
}