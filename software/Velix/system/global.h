//
// Created by 18135 on 26-1-17.
//

#ifndef SPIN_MASTER_V2_GLOBAL_H
#define SPIN_MASTER_V2_GLOBAL_H

#include "motor.h"
#include "PID.h"
#include "command.h"
#include "sample.h"
#include "identify.h"
#include "FOC.h"
#include "encoder.h"
#include "velocity.h"
#include "observer.h"
#include "config.h"
#include "led_task.h"

typedef struct MotorSystem {
    MOTOR motor;

    PIDController pos_pid;
    PIDController HFI_pos_pid;
    PIDController speed_low_pid;
    PIDController speed_pid;   // 速度环 PID
    PIDController iq_pid;      // q轴电流 PID
    PIDController id_pid;

    PIDController HFI_id_pid;
    PIDController HFI_iq_pid;

    PIDController HFI_speed_pid;

    Motor_Command cmd;

    uint16_t zeroEAngle;

    IDENTIFY_STRUCT identify;

    SAMPLE_STRUCT sample;

    FOC_STRUCT foc;

    ENCODER_STRUCT encoder;

    VELOCITY_STRUCT velocity;

    PLL_STRUCT  SPLL;

    PLL_STRUCT HPLL;

    SMO_STRUCT  SMO;

    HFI_STRUCT HFI;

    HFI_SMO_SWITCH HfiSmoSwitch;
}MotorSystem;

typedef struct
{
    uint8_t   RunState;          // 运行状态
    uint8_t   RunMode;           // 运行模式
}MOTOR_STRUCT;

extern MotorSystem Mt;
extern volatile uint32_t sys_tick_ms;          // 每1ms +1
extern volatile uint32_t task_counter;         // 用于分频

void System_Init(void);
void System_Loop(void);
void FOC_Loop(MotorSystem *p);

#endif //SPIN_MASTER_V2_GLOBAL_H
