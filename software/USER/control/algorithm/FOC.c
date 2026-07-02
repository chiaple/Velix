//
// Created by 18135 on 26-1-17.
//

#include "FOC.h"
#include "config.h"
#include "global.h"

/**----------------------------------------------------

                将角度归化到 0 到 2PI

------------------------------------------------------*/
float32_t normalizeAngle(float32_t angle)
{
    angle = fmodf(angle, 2 * PI);  // 取模运算
    return angle < 0 ? angle + 2 * PI : angle; // 处理负角度
}
/**----------------------------------------------------

                将角度归化到 0 到 360

------------------------------------------------------*/
float32_t normalizeDegAngle(float32_t angle)
{
    angle = fmodf(angle, 360);  // 取模运算
    return angle < 0 ? angle + 360 : angle; // 处理负角度
}
/**----------------------------------------------------

                        获取电角度

------------------------------------------------------*/
float32_t GetElectric_Angle(const ENCODER_STRUCT *encoder, const MOTOR *motor)
{
    const float32_t rawAngle = encoder->encoderRaw - encoder->zeroEAngle;
    const float32_t electric_float32Angle = normalizeAngle(rawAngle * 2 * PI / encoder->EncoderValMax * motor->polePairs * motor->dir);
    return electric_float32Angle;
}
/**----------------------------------------------------

                        Clarke变换（3 -> 2）

------------------------------------------------------*/
void Clarke_Transform(const float32_t iu, float32_t iv, const float32_t iw, float32_t* i_alpha, float32_t* i_beta)
{
    /* 采用等幅值变换
     * 变换矩阵：
     * [ α ] = [     1             0     ] [ u ]
     * [ β ] = [ -1/sqrt(3)   -2/sqrt(3) ] [ V ]
     */
    *i_alpha = iu;
    *i_beta  = -(iu + 2 * iw) * _1_SQRT3;
}
/**----------------------------------------------------

                        Park变换（静止坐标系 -> 旋转坐标系）

------------------------------------------------------*/
void Park_Transform(const float32_t i_alpha, const float32_t i_beta, const float32_t e_Angle, float32_t* id, float32_t* iq)
{
    const float32_t sinTheta = arm_sin_f32(e_Angle);
    const float32_t cosTheta = arm_cos_f32(e_Angle);
    /* 变换矩阵：
     * [ Id ] = [ cosθ    sinθ ] [ α ]
     * [ Iq ] = [ sinθ   -cosθ ] [ β ]
     */
    *id = i_alpha * cosTheta + i_beta * sinTheta;
    *iq = i_alpha * (-sinTheta) + i_beta * cosTheta;
}

/**----------------------------------------------------

					计算SVPWM

------------------------------------------------------*/
SVPWM CalculateSVPWM(FOC_STRUCT *foc, const float32_t Uq, const float32_t Ud, const float32_t eAngle) {
    const float32_t Ts = 1.0f;
    const float32_t	TsDivUdc = Ts / foc->U_bus;
    float32_t Tx, Ty;

    uint8_t N = 0;
    float32_t sinAngle = 0.0f;
    float32_t cosAngle = 0.0f;
    float32_t Tcmp1 = 0.0f;
    float32_t Tcmp2 = 0.0f;
    float32_t Tcmp3 = 0.0f;

    sinAngle = arm_sin_f32(eAngle);
    cosAngle = arm_cos_f32(eAngle);
    /*****反Park变换*****/
    foc->U_alpha = Ud * cosAngle - Uq * sinAngle;
    foc->U_beta = Ud * sinAngle + Uq * cosAngle;
    /**扇区判断*/
    if (foc->U_beta > 0.0f)
        N = 1;
    if ((1.73205078f * foc->U_alpha - foc->U_beta) / 2.0f > 0.0f)
        N += 2;
    if ((-1.73205078f * foc->U_alpha - foc->U_beta) / 2.0f > 0.0f)
        N += 4;
    /**
     * 关系值N     3   1   5   4   6   2
     * 扇区       1   2   3   4   5   6
     * */
    switch (N)
    {
        case 1:
            Tx = (-1.5f * foc->U_alpha + 0.866025388f * foc->U_beta) * TsDivUdc;
            Ty = (1.5f * foc->U_alpha + 0.866025388f * foc->U_beta) * TsDivUdc;
            break;

        case 2:
            Tx = (1.5f * foc->U_alpha + 0.866025388f * foc->U_beta) * TsDivUdc;
            Ty = -(1.73205078f * foc->U_beta * TsDivUdc);
            break;

        case 3:
            Tx = -((-1.5f * foc->U_alpha + 0.866025388f * foc->U_beta) * TsDivUdc);
            Ty = 1.73205078f * foc->U_beta * TsDivUdc;
            break;

        case 4:
            Tx = -(1.73205078f * foc->U_beta * TsDivUdc);
            Ty = (-1.5f * foc->U_alpha + 0.866025388f * foc->U_beta) * TsDivUdc;
            break;

        case 5:
            Tx = 1.73205078f * foc->U_beta * TsDivUdc;
            Ty = -((1.5f * foc->U_alpha + 0.866025388f * foc->U_beta) * TsDivUdc);
            break;

        default:
            Tx = -((1.5f * foc->U_alpha + 0.866025388f * foc->U_beta) * TsDivUdc);
            Ty = -((-1.5f * foc->U_alpha + 0.866025388f * foc->U_beta) * TsDivUdc);
            break;
    }

    const float32_t f_temp = Tx + Ty;
    if (f_temp > Ts)
    {
        Tx /= f_temp;
        Ty /= f_temp;
    }
    //将7段式SVPWM转换为占空比
    const float32_t Ta = (Ts - (Tx + Ty)) / 4.0f;
    const float32_t Tb = Tx / 2.0f + Ta;
    const float32_t Tc = Ty / 2.0f + Tb;
    switch (N) {
        case 1:
            Tcmp1 = Tb;
            Tcmp2 = Ta;
            Tcmp3 = Tc;
            break;

        case 2:
            Tcmp1 = Ta;
            Tcmp2 = Tc;
            Tcmp3 = Tb;
            break;

        case 3:
            Tcmp1 = Ta;
            Tcmp2 = Tb;
            Tcmp3 = Tc;
            break;

        case 4:
            Tcmp1 = Tc;
            Tcmp2 = Tb;
            Tcmp3 = Ta;
            break;

        case 5:
            Tcmp1 = Tc;
            Tcmp2 = Ta;
            Tcmp3 = Tb;
            break;

        case 6:
            Tcmp1 = Tb;
            Tcmp2 = Tc;
            Tcmp3 = Ta;
            break;
    }
    const SVPWM outputs = (SVPWM){Tcmp1 * TIM1_ARR, Tcmp2 * TIM1_ARR, Tcmp3 * TIM1_ARR};
    return outputs;
}

/**----------------------------------------------------

					生成PWM

------------------------------------------------------*/
void setSVPWM(const int motorNum, const float32_t Uq, const float32_t Ud, const float32_t eAngle) {

    if (motorNum == 0) {
        const SVPWM motor_OUT = CalculateSVPWM(&Mt.foc, Uq, Ud, eAngle);
        VELIX_PWM_SET_U((uint32_t)motor_OUT.Tim_cmpU);
        VELIX_PWM_SET_V((uint32_t)motor_OUT.Tim_cmpV);
        VELIX_PWM_SET_W((uint32_t)motor_OUT.Tim_cmpW);
    }
}
