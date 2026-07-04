//
// Created by 18135 on 26-1-17.
//

#include "command.h"


void MotorCommand_Init(Motor_Command *cmd){
    if(cmd == NULL) return;

    cmd->position  = 0.0f;
    cmd->fPosition = 0.0f;
    cmd->speed     = 0.0f;
    cmd->current_d = 0.0f;
    cmd->current_q = 0.0f;
    cmd->mode      = CONTROL_MODE_IDLE;
    cmd->state     = MOTOR_STOP;
}

void ResetCmd(Motor_Command *cmd)
{
    if (cmd == NULL) return;

    cmd->position  = 0.0f;
    cmd->fPosition = 0.0f;
    cmd->speed     = 0.0f;
    cmd->current_d = 0.0f;
    cmd->current_q = 0.0f;
    cmd->fSpeed    = 0.0f;
}

void UpdateTransitionTarget(float *transition_target, float final_target, float rate) {
    // 计算当前过渡目标值与最终目标值之间的差距
    if(final_target==*transition_target) return; // 已经达到目标值，无需调整
    float delta = final_target - *transition_target;

    // 逐步调整过渡目标值，使其朝最终目标值过渡
    if (delta > 0) {
        *transition_target += delta * rate;
        if (*transition_target > final_target) {
            *transition_target = final_target;  // 防止超出最终目标值
        }
    } else {
        *transition_target += delta * rate;
        if (*transition_target < final_target) {
            *transition_target = final_target;  // 防止超出最终目标值
        }
    }
}

void UpdateTransitionTarget_Linear(float *transition_target, float final_target, float step) {
    if (*transition_target < final_target) {
        *transition_target += step;
        if (*transition_target > final_target) {
            *transition_target = final_target;  // 防止超过目标值
        }
    } else if (*transition_target > final_target) {
        *transition_target -= step;
        if (*transition_target < final_target) {
            *transition_target = final_target;  // 防止超过目标值
        }
    }
}
