//
// Created by 18135 on 26-1-17.
//

#ifndef SPIN_MASTER_V2_FOC_H
#define SPIN_MASTER_V2_FOC_H

#include "arm_math.h"
#include "config.h"
#include "motor.h"
#include "encoder.h"


typedef struct {

    float32_t 			Tim_cmpU;//U相占空比
    float32_t 			Tim_cmpV;//V相占空比
    float32_t 			Tim_cmpW;//W相占空比
}SVPWM;

typedef struct {

    float U_bus;          // 母线电压

    float32_t iu;
    float32_t iv;
    float32_t iw;

    float32_t i_alpha;
    float32_t i_beta;

    float32_t id;		//d轴电流反馈
    float32_t iq;		//q轴电流反馈
    float32_t Ud;		//d轴电压输出
    float32_t Uq;		//q轴电压输出

    float32_t U_alpha;
    float32_t U_beta;

    float32_t IdLPF; 	       // d轴电流滤波值
    float32_t IqLPF; 		   // q轴电流滤波值

    float   IdLPFFactor;
    float   IqLPFFactor;

    float32_t eAngle;

}FOC_STRUCT;



float32_t normalizeAngle(float32_t angle);
float32_t normalizeDegAngle(float32_t angle);
float32_t GetElectric_Angle(const ENCODER_STRUCT *encoder, const MOTOR *motor);
void Clarke_Transform(const float32_t iu, float32_t iv, const float32_t iw, float32_t* i_alpha, float32_t* i_beta);
void Park_Transform(const float32_t i_alpha, const float32_t i_beta, const float32_t e_Angle, float32_t* id, float32_t* iq);
SVPWM CalculateSVPWM(FOC_STRUCT *foc, const float32_t Uq, const float32_t Ud, const float32_t eAngle);
void setSVPWM(const int motorNum, const float32_t Uq, const float32_t Ud, const float32_t eAngle);

#endif //SPIN_MASTER_V2_FOC_H
