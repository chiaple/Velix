//
// Created by e on 26-1-22.
//

#include "encoder.h"
#include "MT6701.h"
#include "FOC.h"

/**
  * 函数功能:电角度发生器
  * 输入参数:
  * 返回参数:
  * 说    明:根据设定的电角速度速度生成电角度
  */
void Electrical_Angle_Generator(ENCODER_STRUCT *p)
{
    // 电转速 rpm → rad/s → 每50μs的增量
    float delta = p->ElectricalSpdSet * (2 * PI / 60.0f) * 0.00005f;

    p->elecAngleRad += delta;

    // 限制在 [0, 2π)
    if (p->elecAngleRad >= 2 * PI) {
        p->elecAngleRad -= 2 * PI;
    }
    else if (p->elecAngleRad < 0.0f) {
        p->elecAngleRad += 2 * PI;
    }
}
/**
 * 读取角度信息
 * */
void ReadAngleDeg(ENCODER_STRUCT *p){
    p->encoderRaw = MT6701_ReadRaw();//原始数据
    p->degAngle = (float32_t)p->encoderRaw * 360.0f / p->EncoderValMax;
    p->angle = (float32_t)p -> encoderRaw * 2.0f * PI / p->EncoderValMax;
}
/**
 * 位置计算
 * */
void Get_position(ENCODER_STRUCT *p){
    static float32_t prevDeg = 0.0f;

    p->curDegAngle = normalizeDegAngle((float32_t)(p->encoderRaw - p->zeroEAngle) * 360.0f / p->EncoderValMax);

    float delta = p->curDegAngle - prevDeg;
    if (fabsf(delta) > 180.0f)
    {	// 角度跳变超过180°时判定为跨圈
        if (delta > 0)
        {
            p->numOfCircles--;  // 顺时针跨圈（如360°→ 0°）
        } else
        {
            p->numOfCircles++;  // 逆时针跨圈（如0°→ 360°）
        }
    }
    prevDeg = p->curDegAngle;

    p->absDegAngle = p->curDegAngle + (float32_t)p->numOfCircles * 360.0f;
}


