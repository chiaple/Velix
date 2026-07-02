//
// Created by e on 26-1-17.
//

#ifndef SPIN_MASTER_V2_PID_H
#define SPIN_MASTER_V2_PID_H

#include "arm_math.h"
#define CLAMP(x, min, max) ((x) < (min) ? (min) : ((x) > (max) ? (max) : (x)))

typedef struct{
    float32_t Kp;						// 比例系数
    float32_t Ki; 						// 积分系数
    float32_t Kd;						// 微分系数
    float32_t errorSum;     			// 误差积分
    float32_t lastError;    			// 上次误差
    float32_t outputMin;    			// 输出最小值
    float32_t outputMax;    			// 输出最大值
    float32_t integralMin, integralMax; // 积分限幅参数
    float32_t dt;						// 采样时间
    float32_t inv_dt;					// 采样时间的倒数

    float32_t error;
    float32_t output;
} PIDController;




void PID_init(PIDController* pid, const float32_t Kp, const float32_t Ki, const float32_t Kd, const float32_t outputMin, const float32_t outputMax, const float32_t imin, const float32_t imax);
float32_t PID(PIDController* pid, const float32_t setpoint, const float32_t input);

// Reset PID internal state (clear integral and last error)
void PID_reset(PIDController* pid);

#endif //SPIN_MASTER_V2_PID_H
