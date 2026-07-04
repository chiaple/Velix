//
// Created by e on 26-1-26.
//

#include "sensorless.h"
#include "global.h"

static uint8_t sensorless_step = 0;                 // 运行步骤
static uint8_t sensorless_errorcnt = 0;             // 失败次数
static uint16_t sensorless_runtim = 0;              // 运行时间
static uint16_t sensorless_comparecnt = 0;          // 比较计数
static uint16_t sensorless_successcnt = 0;          // 正确次数
static uint16_t sensorless_errortimout = 0;         // 超时报错

static void Sensorless_StrongDragVoltageOpenLoop(MotorSystem *p);
static void Sensorless_StrongDragCurrentClose(MotorSystem *p);
static void Sensorless_StrongDragSmoSpeedCurrentLoop(MotorSystem *p);
static void Sensorless_HfiSpeedCurrentLoop(MotorSystem *p);
static void Sensorless_HfiSmoAdaptiveLoop(MotorSystem *p);
static void Sensorless_Idle(MotorSystem *p);
static void Sensorless_UpdateHfiAccumulatedPosition(MotorSystem *p);
void HfiSmo_UpdateAdaptiveSwitch(HFI_SMO_SWITCH *p);

static void Sensorless_UpdateHfiAccumulatedPosition(MotorSystem *p)
{
    float32_t delta = 0.0f;
    const float32_t hfi_theta = p->HPLL.ThetaCompensate;

    if (p->encoder.HfiPositionInit == 0U)
    {
        p->encoder.HfiPositionInit = 1U;
        p->encoder.HfiLastElecAngleRad = hfi_theta;
        p->encoder.HfiAbsDegAngle = 0.0f;
        p->encoder.absDegAngle = 0.0f;
        return;
    }

    delta = hfi_theta - p->encoder.HfiLastElecAngleRad;
    delta = delta > PI ? delta - 2.0f * PI : delta;
    delta = delta < -PI ? delta + 2.0f * PI : delta;

    p->encoder.HfiLastElecAngleRad = hfi_theta;
    p->encoder.HfiAbsDegAngle += delta * 180.0f / PI / (float32_t)p->motor.polePairs;
    p->encoder.absDegAngle = p->encoder.HfiAbsDegAngle;
}

// 根据当前无感模式分发到对应处理函数。
void Sensorless_Control(MotorSystem *p)
{
    switch (p->cmd.mode)
    {
        case STRONG_DRAG_CURRENT_OPEN:
            Sensorless_StrongDragVoltageOpenLoop(p);
            break;
        case STRONG_DRAG_CURRENT_CLOSE:
            Sensorless_StrongDragCurrentClose(p);
            break;
        case STRONG_DRAG_SMO_SPEED_CURRENT_LOOP:
            Sensorless_StrongDragSmoSpeedCurrentLoop(p);
            break;
        case HFI_CURRENT_CLOSE:
            Sensorless_HfiSpeedCurrentLoop(p);
            break;
        case HFI_SMO_SPEED_CURRENT_LOOP:
            Sensorless_HfiSmoAdaptiveLoop(p);
            break;
        case CONTROL_MODE_IDLE:
            Sensorless_Idle(p);
            break;
        default:
            break;
    }
}

// 开环强拖，通过给定固定电角速度带动电机起转。
static void Sensorless_StrongDragVoltageOpenLoop(MotorSystem *p)
{
    p->foc.Ud = SENSORLESS_STRONG_DRAG_UD_DEFAULT;
    p->encoder.ElectricalSpdSet = SENSORLESS_STRONG_DRAG_ELEC_RPM_DEFAULT;
    Electrical_Angle_Generator(&p->encoder);
}

// 电流闭环强拖，用 d 轴电流建立起转时的磁场。
static void Sensorless_StrongDragCurrentClose(MotorSystem *p)
{
    p->cmd.current_d = SENSORLESS_STRONG_DRAG_CURRENT_D_DEFAULT;
    p->encoder.ElectricalSpdSet = SENSORLESS_STRONG_DRAG_ELEC_RPM_DEFAULT;
    Electrical_Angle_Generator(&p->encoder);

    Clarke_Transform(p->foc.iu, p->foc.iv, p->foc.iw, &p->foc.i_alpha, &p->foc.i_beta);
    Park_Transform(p->foc.i_alpha, p->foc.i_beta, p->encoder.elecAngleRad, &p->foc.id, &p->foc.iq);

    p->foc.IqLPF = p->foc.iq * p->foc.IqLPFFactor + p->foc.IqLPF * (1 - p->foc.IqLPFFactor);
    p->foc.IdLPF = p->foc.id * p->foc.IdLPFFactor + p->foc.IdLPF * (1 - p->foc.IdLPFFactor);

    p->foc.Uq = PID(&p->iq_pid, p->cmd.current_q, p->foc.IqLPF);
    p->foc.Ud = PID(&p->id_pid, p->cmd.current_d, p->foc.IdLPF);
}

// 强拖切滑膜模式，用状态机完成启动、比较和切换。
static void Sensorless_StrongDragSmoSpeedCurrentLoop(MotorSystem *p)
{
    p->velocity.MechanicalSpeedSet = SENSORLESS_STRONG_DRAG_MECH_RPM_DEFAULT;

    if (sensorless_step == 0)
    {
        p->cmd.current_q = 0;
        p->cmd.current_d = 0;
        p->speed_pid.errorSum = 0;
        p->encoder.ElectricalValSet = 0;
        p->velocity.MechanicalSpeedSetLast = 0;
        p->SPLL.WeForeLPF = 0;
        p->SPLL.IPart = 0;

        p->encoder.angle = 0;
        p->encoder.lastAngle = 0;
        p->encoder.velocity = 0;
        p->encoder.RPM = 0;

        if (p->velocity.MechanicalSpeedSet >= SENSORLESS_SWITCH_MECH_RPM_BASE_DEFAULT / p->motor.polePairs ||
            p->velocity.MechanicalSpeedSet <= -SENSORLESS_SWITCH_MECH_RPM_BASE_DEFAULT / p->motor.polePairs)
        {
            sensorless_step = 1;
        }
    }
    else if (sensorless_step == 1)
    {
        p->cmd.current_q = 0;
        p->cmd.current_d = SENSORLESS_STRONG_DRAG_ALIGN_CURRENT_D_DEFAULT;
        p->encoder.ElectricalSpdSet = p->velocity.MechanicalSpeedSet * p->motor.polePairs;
        Electrical_Angle_Generator(&p->encoder);

        if (p->encoder.ElectricalSpdSet >= 0) p->SPLL.Dir = -1;
        if (p->encoder.ElectricalSpdSet < 0)  p->SPLL.Dir = 1;

        if (p->encoder.elecAngleRad > SENSORLESS_COMPARE_ANGLE_MIN_RAD_DEFAULT &&
            p->encoder.elecAngleRad < SENSORLESS_COMPARE_ANGLE_MAX_RAD_DEFAULT)
        {
            if (p->SPLL.ThetaCompensate > SENSORLESS_COMPARE_ANGLE_MIN_RAD_DEFAULT &&
                p->SPLL.ThetaCompensate < SENSORLESS_COMPARE_ANGLE_MAX_RAD_DEFAULT)
            {
                sensorless_comparecnt++;
                if (p->encoder.elecAngleRad - p->SPLL.ThetaCompensate <= 2 * PI * SENSORLESS_COMPARE_ERROR_RATIO_DEFAULT)
                {
                    if (p->encoder.elecAngleRad - p->SPLL.ThetaCompensate >= -2 * PI * SENSORLESS_COMPARE_ERROR_RATIO_DEFAULT)
                    {
                        sensorless_successcnt++;
                    }
                }
            }
        }

        if (sensorless_comparecnt >= SENSORLESS_COMPARE_COUNT_DEFAULT)
        {
            if (sensorless_successcnt >= sensorless_comparecnt * SENSORLESS_COMPARE_SUCCESS_RATIO_DEFAULT)
            {
                sensorless_step = 2;
                p->cmd.current_d = 0;
                p->encoder.ElectricalValSet = 0;
            }
            sensorless_successcnt = 0;
            sensorless_comparecnt = 0;
        }

        sensorless_errortimout++;
        if (sensorless_errortimout >= SENSORLESS_STARTUP_TIMEOUT_DEFAULT)
        {
            sensorless_errortimout = 0;
            sensorless_errorcnt++;
        }
    }
    else if (sensorless_step == 2)
    {
        p->velocity.SpeedCalculateCnt++;
        if (p->velocity.SpeedCalculateCnt >= SENSORLESS_SPEED_LOOP_DIV_DEFAULT)
        {
            p->velocity.SpeedCalculateCnt = 0;

            p->encoder.RPM = p->SPLL.WeForeLPF / (2.0f * PI) * 60.0f;
            p->encoder.MechanicalSpeed = p->encoder.RPM / p->motor.polePairs;

            if (p->cmd.speed >= 0 && p->cmd.speed <= SENSORLESS_MIN_RUNNING_RPM_DEFAULT) {
                p->cmd.speed = SENSORLESS_MIN_RUNNING_RPM_DEFAULT;
            }
            if (p->cmd.speed <= 0 && p->cmd.speed >= -SENSORLESS_MIN_RUNNING_RPM_DEFAULT) {
                p->cmd.speed = -SENSORLESS_MIN_RUNNING_RPM_DEFAULT;
            }
            p->cmd.current_q = PID(&p->speed_pid, p->cmd.speed, p->encoder.MechanicalSpeed);
        }

        p->encoder.elecAngleRad = p->SPLL.ThetaCompensate;
        if (sensorless_runtim < SENSORLESS_BLOCK_CHECK_DELAY_DEFAULT)
        {
            sensorless_runtim++;
        }
        if (sensorless_runtim >= SENSORLESS_BLOCK_CHECK_DELAY_DEFAULT)
        {
            if (p->encoder.RPM <= SENSORLESS_BLOCK_CHECK_RPM_BASE_DEFAULT &&
                p->encoder.RPM >= -SENSORLESS_BLOCK_CHECK_RPM_BASE_DEFAULT)
            {
                sensorless_step = 0;
                sensorless_runtim = 0;
                sensorless_errorcnt++;
            }
        }
    }

    Clarke_Transform(p->foc.iu, p->foc.iv, p->foc.iw, &p->foc.i_alpha, &p->foc.i_beta);
    Park_Transform(p->foc.i_alpha, p->foc.i_beta, p->encoder.elecAngleRad, &p->foc.id, &p->foc.iq);

    p->foc.IqLPF = p->foc.iq * p->foc.IqLPFFactor + p->foc.IqLPF * (1 - p->foc.IqLPFFactor);
    p->foc.IdLPF = p->foc.id * p->foc.IdLPFFactor + p->foc.IdLPF * (1 - p->foc.IdLPFFactor);

    p->foc.Uq = PID(&p->iq_pid, p->cmd.current_q, p->foc.IqLPF);
    p->foc.Ud = PID(&p->id_pid, p->cmd.current_d, p->foc.IdLPF);

    p->SMO.Ialpha = p->foc.i_alpha;
    p->SMO.Ibeta = p->foc.i_beta;
    p->SMO.Ualpha = p->foc.U_alpha;
    p->SMO.Ubeta = p->foc.U_beta;
    SMO_Calculate(&p->SMO);

    p->SPLL.Ain = p->SMO.EalphaForeLPF;
    p->SPLL.Bin = p->SMO.EbetaForeLPF;
    p->SPLL.SinVal = arm_sin_f32(p->SPLL.ThetaFore);
    p->SPLL.CosVal = arm_cos_f32(p->SPLL.ThetaFore);
    PLL_Calculate(&p->SPLL);
}

// 高频注入速度电流闭环，用 HFI 估算角度和速度。
static void Sensorless_HfiSpeedCurrentLoop(MotorSystem *p)
{
    Clarke_Transform(p->foc.iu, p->foc.iv, p->foc.iw, &p->foc.i_alpha, &p->foc.i_beta);
    Park_Transform(p->foc.i_alpha, p->foc.i_beta, p->HPLL.ThetaCompensate, &p->foc.id, &p->foc.iq);

    p->HFI.Id = p->foc.id;
    p->HFI.Iq = p->foc.iq;
    p->HFI.Ibeta = p->foc.i_beta;
    p->HFI.Ialpha = p->foc.i_alpha;

    HFI_Calculate(&p->HFI);

    if (p->HFI.NSDFlag == 0)
    {
        p->cmd.current_d = p->HFI.IdRef;
    }

    if (p->HFI.NSDOut == 1)
    {
        p->HFI.NSDOut = 0;
        p->HPLL.ThetaFore += PI;
        if (p->HPLL.ThetaFore > 2 * PI)
        {
            p->HPLL.ThetaFore -= 2 * PI;
        }
    }

    p->HPLL.SinVal = arm_sin_f32(p->HPLL.ThetaFore);
    p->HPLL.CosVal = arm_cos_f32(p->HPLL.ThetaFore);
    p->HPLL.Ain = p->HFI.IbetaOut;
    p->HPLL.Bin = -p->HFI.IalphaOut;
    PLL_Calculate(&p->HPLL);

    p->encoder.elecAngleRad = p->HPLL.ThetaCompensate;
    if (p->HFI.NSDFlag != 0)
    {
        Sensorless_UpdateHfiAccumulatedPosition(p);
    }
    else
    {
        p->encoder.HfiPositionInit = 0U;
    }

    p->encoder.PosCalculateCnt++;
    if(p->encoder.PosCalculateCnt>=4){
        p->encoder.PosCalculateCnt = 0;
        p->cmd.speed = PID(&p->HFI_pos_pid, p->cmd.position, p->encoder.absDegAngle);
    }

    p->velocity.SpeedCalculateCnt++;
    if (p->velocity.SpeedCalculateCnt >= HFI_SPEED_LOOP_DIV_DEFAULT)
    {
        p->velocity.SpeedCalculateCnt=0;
        p->cmd.current_q = PID(&p->HFI_speed_pid, p->cmd.speed, p->HPLL.WeForeLPF / (2.0f * PI) * 60.0f / (float)p->motor.polePairs);
    }

    p->foc.IqLPF = p->HFI.IqBase * p->foc.IqLPFFactor + p->foc.IqLPF * (1 - p->foc.IqLPFFactor);
    p->foc.IdLPF = p->HFI.IdBase * p->foc.IdLPFFactor + p->foc.IdLPF * (1 - p->foc.IdLPFFactor);

    p->foc.Uq = PID(&p->iq_pid, p->cmd.current_q, p->foc.IqLPF);
    p->foc.Ud = PID(&p->HFI_id_pid, p->cmd.current_d, p->foc.IdLPF);
    p->foc.Ud += p->HFI.Uin;
}

// HFI & SMO
static void Sensorless_HfiSmoAdaptiveLoop(MotorSystem *p)
{
    Clarke_Transform(p->foc.iu, p->foc.iv, p->foc.iw, &p->foc.i_alpha, &p->foc.i_beta);
    Park_Transform(p->foc.i_alpha, p->foc.i_beta, p->HfiSmoSwitch.OutputThetaRad, &p->foc.id, &p->foc.iq);

    p->SMO.Ialpha = p->foc.i_alpha;
    p->SMO.Ibeta  = p->foc.i_beta;
    p->SMO.Ualpha = p->foc.U_alpha;
    p->SMO.Ubeta  = p->foc.U_beta;
    SMO_Calculate(&Mt.SMO);

    p->SPLL.Dir = (p->HfiSmoSwitch.OutputEleSpeedRpm >= 0.0f) ? -1 : 1;

    p->SPLL.Ain = p->SMO.EalphaForeLPF;
    p->SPLL.Bin = p->SMO.EbetaForeLPF;
    p->SPLL.SinVal = arm_sin_f32(p->SPLL.ThetaFore);
    p->SPLL.CosVal = arm_cos_f32(p->SPLL.ThetaFore);
    PLL_Calculate(&p->SPLL);

    if(p->HFI.Enable == 1) {
        //高频注入模式
        p->HFI.Id = p->foc.id;
        p->HFI.Iq = p->foc.iq;
        p->HFI.Ibeta = p->foc.i_beta;
        p->HFI.Ialpha = p->foc.i_alpha;
        HFI_Calculate(&p->HFI);

        //极性辨识未完成
        if (p->HFI.NSDFlag == 0)
        {
            p->cmd.current_d = p->HFI.IdRef;
        }
        //极性辨识结果表示需要翻转角度
        if (p->HFI.NSDOut == 1)
        {
            p->HFI.NSDOut = 0;
            p->HPLL.ThetaFore += PI;
            if (p->HPLL.ThetaFore > 2 * PI)
            {
                p->HPLL.ThetaFore -= 2 * PI;
            }
        }
        //PLL
        p->HPLL.SinVal = arm_sin_f32(p->HPLL.ThetaFore);
        p->HPLL.CosVal = arm_cos_f32(p->HPLL.ThetaFore);
        p->HPLL.Ain = p->HFI.IbetaOut;
        p->HPLL.Bin = -p->HFI.IalphaOut;
        PLL_Calculate(&p->HPLL);
    }

    //--------切换逻辑
    p->HfiSmoSwitch.HfiEleSpeedRpm = p->HPLL.WeForeLPF / (2.0f * PI) * 60.0f;  //HFI速度（电）
    p->HfiSmoSwitch.HfiThetaRad    = p->HPLL.ThetaFore;                        //HFI角度（电）

    p->HfiSmoSwitch.SmoEleSpeedRpm = p->SPLL.WeForeLPF / (2.0f * PI) * 60.0f;  //SMO速度（电）
    p->HfiSmoSwitch.SmoThetaRad    = p->SPLL.ThetaFore;                        //SMO角度（电）

    p->HfiSmoSwitch.SpeedRefEleRpm = p->cmd.speed * (float)p->motor.polePairs;  //速度参考值（电）

    HfiSmo_UpdateAdaptiveSwitch(&p->HfiSmoSwitch);
    //--------切换逻辑

    //--------速度环
    p->velocity.SpeedCalculateCnt++;
    if (p->velocity.SpeedCalculateCnt >= HFI_SPEED_LOOP_DIV_DEFAULT)
    {
        p->velocity.SpeedCalculateCnt = 0;

        p->cmd.current_q = PID(&p->speed_pid, p->cmd.speed, p->HfiSmoSwitch.OutputEleSpeedRpm / (float)p->motor.polePairs);
    }
    //--------速度环
    //--------电流环
    if(p->HFI.Enable == 0) {
        //SMO
        p->foc.IqLPF = p->foc.iq * p->foc.IqLPFFactor + p->foc.IqLPF * (1 - p->foc.IqLPFFactor);
        p->foc.IdLPF = p->foc.id * p->foc.IdLPFFactor + p->foc.IdLPF * (1 - p->foc.IdLPFFactor);

    }
    else {
        //HFI
        p->foc.IqLPF = p->HFI.IqBase * p->foc.IqLPFFactor + p->foc.IqLPF * (1 - p->foc.IqLPFFactor);
        p->foc.IdLPF = p->HFI.IdBase * p->foc.IdLPFFactor + p->foc.IdLPF * (1 - p->foc.IdLPFFactor);
    }
    p->foc.Uq = PID(&p->iq_pid, p->cmd.current_q, p->foc.IqLPF);
    p->foc.Ud = PID(&p->id_pid, p->cmd.current_d, p->foc.IdLPF);

    if(p->HFI.Enable == 1)
    {
        // 使能 HFI 时，需要将高频注入电压叠加到 Ud 上
        p->foc.Ud += p->HFI.Uin;
    }
    //--------电流环
    p->encoder.elecAngleRad = p->HfiSmoSwitch.OutputThetaRad;

}
//HFI&SMO切换过程
void HfiSmo_UpdateAdaptiveSwitch(HFI_SMO_SWITCH *p) {

    p->HfiEleSpeedRpmAbs = fabsf(p->HfiEleSpeedRpm);     // HFI 估算速度绝对值（rpm）电
    p->SmoEleSpeedRpmAbs = fabsf(p->SmoEleSpeedRpm);     // SMO 估算速度绝对值（rpm）电
    p->SpeedRefEleRpmAbs = fabsf(p->SpeedRefEleRpm);     // 速度参考值绝对值（rpm）电
    uint8_t speed_dir_match = 0U;

    p->ThetaErrRad = p->HfiThetaRad - p->SmoThetaRad;    // HFI 角度 - SMO 角度（范围0~2PI）

    p->ThetaErrRad = p->ThetaErrRad > PI ? p->ThetaErrRad - 2 * PI : p->ThetaErrRad;
    p->ThetaErrRad = p->ThetaErrRad < -PI ? p->ThetaErrRad + 2 * PI : p->ThetaErrRad;

    // 反转过零附近允许速度符号不稳定；速度起来后，HFI 和 SMO 方向必须一致才累加切换置信度。
    speed_dir_match = ((p->HfiEleSpeedRpm * p->SmoEleSpeedRpm) >= 0.0f) ||
                      (p->HfiEleSpeedRpmAbs <= p->HfiOnlySpeedLimit) ||
                      (p->SmoEleSpeedRpmAbs <= p->HfiOnlySpeedLimit);

    //切换依据，误差小于5%（0.05*2PI），且两种观测速度方向一致
    p->StableCount = (fabsf(p->ThetaErrRad) <= 0.1f * PI && speed_dir_match != 0U) ? p->StableCount + 1 : p->StableCount - 1;

    //STATE: 1 低速
    if((p->HfiEleSpeedRpmAbs <= p->HfiOnlySpeedLimit || p->SmoEleSpeedRpmAbs <= p->HfiOnlySpeedLimit || p->SpeedRefEleRpmAbs <= p->HfiOnlySpeedLimit)) {
        Mt.HFI.Enable = 1;                               // 使能 HFI
        p->OutputEleSpeedRpm = p->HfiEleSpeedRpm;        // 输出速度 = HFI 估算速度
        p->OutputThetaRad = p->HfiThetaRad;              // 输出角度 = HFI 估算角度
        p->OutputMode = HFI;                             // 当前观测器模式记录为 HFI
    }
    //STATE: 2 临界速度
    else if((p->SpeedRefEleRpmAbs > p->HfiOnlySpeedLimit && p->SpeedRefEleRpmAbs < p->TransitionHighSpeedLimit) ||
            (p->HfiEleSpeedRpmAbs > p->HfiOnlySpeedLimit && p->HfiEleSpeedRpmAbs < p->TransitionHighSpeedLimit &&
             p->SmoEleSpeedRpmAbs > p->HfiOnlySpeedLimit && p->SmoEleSpeedRpmAbs < p->TransitionHighSpeedLimit)) {
        // ----- 当前模式为 HFI（正在从 HFI 向 SMO 过渡）-----
        if(p->OutputMode == HFI)
        {
            if(p->StableCount >= 100)              // 角度误差持续很小（收敛），可以切换到 SMO
            {
                p->OutputEleSpeedRpm = p->SmoEleSpeedRpm;    // 输出 SMO 速度
                p->OutputThetaRad = p->SmoThetaRad;          // 输出 SMO 角度
                p->OutputMode = SMO;
            }
            else if(p->StableCount <= 30)          // 角度误差较大，仍使用 HFI
            {
                p->OutputEleSpeedRpm = p->HfiEleSpeedRpm;
                p->OutputThetaRad = p->HfiThetaRad;
                p->OutputMode = HFI;
            }
            else                                // 中间状态，保持上一周期的输出
            {
                p->OutputEleSpeedRpm = p->OutputEleSpeedRpm;
                p->OutputThetaRad = p->OutputThetaRad;
            }
        }
        // ----- 当前模式为 SMO（正在从 SMO 向 HFI 过渡，例如减速）-----
        else
        {
            if(p->StableCount >= 100)              // 角度误差小，切换到 HFI（准备低速运行）
            {
                p->OutputEleSpeedRpm = p->HfiEleSpeedRpm;
                p->OutputThetaRad = p->HfiThetaRad;
                p->OutputMode = HFI;
            }
            else if(p->StableCount <= 30)          // 角度误差大，保持 SMO
            {
                p->OutputEleSpeedRpm = p->SmoEleSpeedRpm;
                p->OutputThetaRad = p->SmoThetaRad;
                p->OutputMode = SMO;
            }
            else
            {
                p->OutputEleSpeedRpm = p->OutputEleSpeedRpm;
                p->OutputThetaRad = p->OutputThetaRad;
            }
        }
    }
    //STATE: 3 高速
    else {
        //速度太高禁止HFI
        if(p->SmoEleSpeedRpmAbs > (p->TransitionHighSpeedLimit + p->HfiDisableSpeedMargin * 1.5f))
        {
            Mt.HFI.Enable = 0;                  // 关闭 HFI，减少计算负担
        }
        //  SMO 速度回落到略低于上限
        else if(p->SmoEleSpeedRpmAbs < (p->TransitionHighSpeedLimit + p->HfiDisableSpeedMargin))
        {
            Mt.HFI.Enable = 1;
        }
        // 高速下输出 SMO 的速度和角度
        p->OutputEleSpeedRpm = p->SmoEleSpeedRpm;
        p->OutputThetaRad = p->SmoThetaRad;
        p->OutputMode = SMO;                    // 当前模式记录为 SMO
    }
    if(p->StableCount > 150)
    {
        p->StableCount = 150;
    }
    else if(p->StableCount <= 0)
    {
        p->StableCount = 0;            // 下限0
    }
}
// 空闲模式，释放无感控制输出。
static void Sensorless_Idle(MotorSystem *p)
{
    p->encoder.elecAngleRad = 0;
    p->foc.Uq = 0;
    p->foc.Ud = 0;
}
