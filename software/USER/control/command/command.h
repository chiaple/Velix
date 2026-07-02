//
// Created by 18135 on 26-1-17.
//

#ifndef SPIN_MASTER_V2_COMMAND_H
#define SPIN_MASTER_V2_COMMAND_H

#include "arm_math.h"
/*************运行模式***********/
typedef enum {
    CONTROL_MODE_IDLE,          //空闲
    CONTROL_MODE_POSITION,      //位置环
    CONTROL_MODE_SPEED,         //速度环
    CONTROL_MODE_CURRENT,       //电流环

    /****有感控制模式*****/
    ENCODER_CALIB,                        // 编码器校准
    CURRENT_OPEN_LOOP,		              // 电流开环
    CURRENT_CLOSE_LOOP,	                  // 电流闭环
    SPEED_CURRENT_LOOP,                   // 速度闭环
    POS_SPEED_CURRENT_LOOP,	              // 位置闭环

    /*****无感控制模式****/
    STRONG_DRAG_CURRENT_OPEN,             // 电流开环强拖
    STRONG_DRAG_CURRENT_CLOSE,		      // 电流闭环强拖
    STRONG_DRAG_SMO_SPEED_CURRENT_LOOP,   // 强拖切滑膜速度电流闭环
    HFI_CURRENT_CLOSE,                    // 电流闭环高频注入（测试HFI角度收敛效果）
    HFI_SPEED_CURRENT_CLOSE,              // 高频注入速度电流闭环
    HFI_SMO_SPEED_CURRENT_LOOP,           // 高频注入切滑膜速度电流闭环
    HFI_POS_SPEED_CURRENT_CLOSE,          // 高频注入位置速度电流闭环

}Motor_ControlMode;

/*************运行状态***********/
typedef enum {

    MOTOR_STOP,                 //停机

    MOTOR_IDENTIFY,             //参数辨识

    MOTOR_SENSORUSE,            //有感控制
    MOTOR_SENSORLESS,           //无感控制
}Motor_ControlState;

typedef struct {
    float32_t position;
    float32_t fPosition;
    float32_t speed;
    float32_t current_d;
    float32_t current_q;
    float32_t fSpeed;
    Motor_ControlMode mode;
    Motor_ControlState state;

    float32_t test;
}Motor_Command;

void MotorCommand_Init(Motor_Command *cmd);
void ResetCmd(Motor_Command *cmd);
void UpdateTransitionTarget(float *transition_target, float final_target, float rate);
void UpdateTransitionTarget_Linear(float *transition_target, float final_target, float step);

#endif //SPIN_MASTER_V2_COMMAND_H
