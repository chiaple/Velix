//
// Created by e on 26-1-17.
//

#include "PID.h"
#include "arm_math.h"

/********************************************************************************
    PID控制器初始化
*********************************************************************************/
void PID_init(PIDController* pid, const float32_t Kp, const float32_t Ki, const float32_t Kd, const float32_t outputMin, const float32_t outputMax, const float32_t imin, const float32_t imax)
{
    pid->Kp = Kp;
    pid->Ki = Ki;
    pid->Kd = Kd;
    pid->errorSum = 0;
    pid->lastError = 0;
    pid->outputMin = outputMin;
    pid->outputMax = outputMax;
    pid->integralMin = imin;
    pid->integralMax = imax;
    pid->dt = 0.01f;
    pid->inv_dt = 1.0f / pid->dt;
}

/********************************************************************************
    PID计算
*********************************************************************************/
float32_t PID(PIDController* pid, const float32_t setpoint, const float32_t input)
{
    // 计算误差
    pid->error = setpoint - input;
    // 积分限幅
    pid->errorSum += pid->error * pid->dt;
    pid->errorSum = CLAMP(pid->errorSum, pid->integralMin, pid->integralMax);

    // 计算微分项
    const float32_t error_delta = (pid->error - pid->lastError) * pid->inv_dt; // 用乘法代替除法

    // PID计算
    pid->output = pid->Kp * pid->error  + pid->Ki * pid->errorSum  + pid->Kd * error_delta;

    // 输出限幅
    pid->output = pid->output < pid->outputMin ? pid->outputMin : (pid->output > pid->outputMax ? pid->outputMax : pid->output);
    // 保存误差历史
    pid->lastError = pid->error;

    return pid->output;
}

// Reset PID internal state (clear integral and last error)
void PID_reset(PIDController* pid)
{
    //if (pid == NULL) return;
    pid->errorSum = 0.0f;
    pid->lastError = 0.0f;
    pid->error = 0.0f;
}
