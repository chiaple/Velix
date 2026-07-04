//
// Created by 18135 on 26-1-17.
//

#ifndef SPIN_MASTER_V2_MOTOR_H
#define SPIN_MASTER_V2_MOTOR_H

#include "arm_math.h"

typedef struct {

    int Number;
    int dir;
    int polePairs;

//    float32_t id;		//d轴电流反馈
//    float32_t iq;		//q轴电流反馈
//    float32_t Ud;		//d轴电压输出
//    float32_t Uq;		//q轴电压输出
//
//    float32_t iu;
//    float32_t iv;
//    float32_t iw;
//
//
//    float32_t ialpha;
//    float32_t ibeta;


}MOTOR;

void motor_Init(MOTOR *p,int Num,int dir,int polePairs);
#endif //SPIN_MASTER_V2_MOTOR_H
