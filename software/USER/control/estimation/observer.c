//
// Created by e on 26-1-21.
//

#include "observer.h"
#include "config.h"
#include "led_task.h"

/**
  * @brief  高频信号注入（HFI, High Frequency Injection）
  *
  * @param[in,out] p
  *        HFI 结构体指针
  *
  *        【输入参数（由外部更新）】
  *        p->Id            D轴实际电流（A）
  *        p->Iq            Q轴实际电流（A）
  *        p->Ialpha        α轴实际电流（A）
  *        p->Ibeta         β轴实际电流（A）
  *        p->Uin           注入电压幅值（V 或 pu）
  *
  *        【内部状态量（函数内部维护，不可随意清零）】
  *        p->IdLast        上一次 D 轴电流
  *        p->IqLast        上一次 Q 轴电流
  *        p->IalphaLast    上一次 α 轴电流
  *        p->IbetaLast     上一次 β 轴电流
  *        p->IalphaHighLast 上一次 α 轴高频电流分量
  *        p->IbetaHighLast  上一次 β 轴高频电流分量
  *        p->NSDCount      初始角度辨识计数器
  *        p->NSDSum1       正向注入高频响应累加值
  *        p->NSDSum2       反向注入高频响应累加值
  *        p->Dir           高频注入方向标志（0 / 1）
  *
  *        【输出参数（函数内部计算）】
  *        p->IdRef         D轴电流参考（用于高频注入）
  *        p->IdHigh        D轴高频电流分量幅值
  *        p->IdBase        D轴基频电流分量
  *        p->IqBase        Q轴基频电流分量
  *        p->IalphaHigh    α轴高频电流分量
  *        p->IbetaHigh     β轴高频电流分量
  *        p->IalphaOut     α轴高频响应输出
  *        p->IbetaOut      β轴高频响应输出
  *        p->NSDOut        初始角度极性判断结果（0 / 1）
  *        p->NSDFlag       初始角度辨识完成标志
  *
  * @note
  * 1. 本函数用于低速 / 零速条件下的初始电角度辨识
  * 2. 通过 D 轴高频电流注入，利用磁阻各向异性进行位置判断
  * 3. NSD（North–South Detection）用于判断磁极方向
  * 4. 输出的高频响应信号可用于后续角度估算或 PLL/HFI 解调
  */
void HFI_Calculate(HFI_STRUCT *p)
{
    /*****************************初始角度辨识*******************************/
    if(p->NSDFlag == 0)
    {
        p->NSDCount++;
        p->IdHigh = (p->Id - p->IdLast) * 0.5f;           //提取高频电流分量
        if(p->IdHigh < 0)
        {
            p->IdHigh = -p->IdHigh;
        }
        if(p->NSDCount < 400)                             //0  等待观测角度收敛
        {
            p->IdRef = 0.0f;
        }
        else if(p->NSDCount >= 400 && p->NSDCount < 600)  //正
        {
            p->IdRef = HFI_IdRef;
        }
        else if(p->NSDCount >= 600 && p->NSDCount < 610)  //正+采样
        {
            p->IdRef = HFI_IdRef;
            p->NSDSum1 += p->IdHigh;
        }
        else if(p->NSDCount >= 610 && p->NSDCount < 810)  //0
        {
            p->IdRef = 0.0f;
        }
        else if(p->NSDCount >= 810 && p->NSDCount < 1010) //负
        {
            p->IdRef = -HFI_IdRef;
        }
        else if(p->NSDCount >= 1010 && p->NSDCount < 1020)//负+采样
        {
            p->IdRef = -HFI_IdRef;
            p->NSDSum2 += p->IdHigh;
        }
        else if(p->NSDCount == 1020)                      //0
        {
            p->IdRef = 0.0f;
        }
        else if(p->NSDCount == 1021)                      //0
        {
            p->NSDFlag = 1;
            p->NSDCount = 0;
            if(p->NSDSum2 > p->NSDSum1)
            {
                p->NSDOut = 1;

                //LED_SetState(LED_COLOR_GREEN, LED_MODE_CUSTOM_BLINK, 100, 100, 3, 900);
            }
            else
            {
                p->NSDOut = 0;
                //LED_SetState(LED_COLOR_RED, LED_MODE_CUSTOM_BLINK, 100, 100, 3, 900);
            }
        }
    }
    /*****************************信号注入与解析*******************************/
    p->IdBase = (p->Id + p->IdLast) * 0.5f;                //提取D轴基频电流分量
    p->IqBase = (p->Iq + p->IqLast) * 0.5f;    			   //提取Q轴基频电流分量
    p->IdLast = p->Id;
    p->IqLast = p->Iq;

    p->IalphaHighLast = p->IalphaHigh;
    p->IbetaHighLast  = p->IbetaHigh;
    p->IalphaHigh = (p->Ialpha - p->IalphaLast) * 0.5f;    //提取α轴高频电流分量
    p->IbetaHigh  = (p->Ibeta  - p->IbetaLast)  * 0.5f;	   //提取β轴高频电流分量
    p->IalphaLast = p->Ialpha;
    p->IbetaLast  = p->Ibeta;

    if(p->Dir == 0)
    {
        p->IalphaOut = p->IalphaHigh - p->IalphaHighLast;
        p->IbetaOut  = p->IbetaHigh  - p->IbetaHighLast;
        if(p->Uin < 0)
        {
            p->Uin = -p->Uin;
        }
        p->Dir = 1;
    }
    else if(p->Dir == 1)
    {
        p->IalphaOut = p->IalphaHighLast - p->IalphaHigh;
        p->IbetaOut  = p->IbetaHighLast  - p->IbetaHigh;
        if(p->Uin > 0)
        {
            p->Uin = -p->Uin;
        }
        p->Dir = 0;
    }
}

/**
  * @brief  滑模观测器（SMO, Sliding Mode Observer）
  * g
  * @param[in,out] p
  *        SMO 结构体指针
  *
  *        【输入参数（由外部更新）】
  *        p->Ialpha        α轴实际电流（A）
  *        p->Ibeta         β轴实际电流（A）
  *        p->Ualpha        α轴定子电压（V）
  *        p->Ubeta         β轴定子电压（V）
  *        p->Rs            定子电阻（Ω）
  *        p->Ld            定子电感（H）
  *        p->Ts            采样周期（s）
  *        p->Gain          滑模增益
  *        p->EabForeLPFFactor  反电动势低通滤波系数（0~1）
  *
  *        【输出参数（函数内部计算）】
  *        p->IalphaFore    α轴观测电流（A）
  *        p->IbetaFore     β轴观测电流（A）
  *        p->EalphaFore    α轴反电动势估计（V，未滤波）
  *        p->EbetaFore     β轴反电动势估计（V，未滤波）
  *        p->EalphaForeLPF α轴反电动势估计（V，低通滤波后）
  *        p->EbetaForeLPF  β轴反电动势估计（V，低通滤波后）
  *
  * @note
  * 1. 本函数基于 αβ 坐标系实现 SMO
  * 2. 通过滑模函数逼近反电动势
  * 3. 输出的反电动势可用于 PLL / 角度估算
  * 4. 建议在电流环频率下调用
  */
void SMO_Calculate(SMO_STRUCT *p)
{
    //离散化电流预测
    p->IalphaFore += p->Ts * (-p->Rs / p->Ld * p->IalphaFore + (p->Ualpha - p->EalphaForeLPF) / p->Ld);
    p->IbetaFore  += p->Ts * (-p->Rs / p->Ld * p->IbetaFore  + (p->Ubeta  - p->EbetaForeLPF)  / p->Ld);

    if((p->IalphaFore - p->Ialpha) > 1.0f) p->EalphaFore = p->Gain;
    else if((p->IalphaFore - p->Ialpha) < -1.0f) p->EalphaFore = -p->Gain;
    else     p->EalphaFore = p->Gain * (p->IalphaFore - p->Ialpha);

    if     ((p->IbetaFore - p->Ibeta) > 1.0f) p->EbetaFore = p->Gain;
    else if((p->IbetaFore - p->Ibeta) < -1.0f) p->EbetaFore = -p->Gain;
    else     p->EbetaFore = p->Gain * (p->IbetaFore - p->Ibeta);

    p->EalphaForeLPF = p->EalphaFore * p->EabForeLPFFactor + p->EalphaForeLPF * (1 - p->EabForeLPFFactor);
    p->EbetaForeLPF  = p->EbetaFore  * p->EabForeLPFFactor + p->EbetaForeLPF  * (1 - p->EabForeLPFFactor);
}

/**
  * @brief  正交锁相环（PLL, Phase Locked Loop）
  *
  * @param[in,out] p
  *        PLL 结构体指针
  *
  *        【输入参数（由外部更新）】
  *        p->Ain            α轴输入信号（通常为反电动势 Eα 或其归一化值）
  *        p->Bin            β轴输入信号（通常为反电动势 Eβ 或其归一化值）
  *        p->SinVal         sin(θ̂)，由当前观测角度计算得到
  *        p->CosVal         cos(θ̂)，由当前观测角度计算得到
  *        p->Dir            旋转方向（+1 或 -1）
  *        p->Kp             PLL 比例系数
  *        p->Ki             PLL 积分系数
  *        p->Ts             采样周期（s）
  *        p->WeForeLPFFactor  角速度低通滤波系数（0~1）
  *
  *        【输出参数（函数内部计算）】
  *        p->ThetaErr       相位误差
  *        p->PPart          PLL 比例项
  *        p->IPart          PLL 积分项（内部状态）
  *        p->WeFore         估计电角速度（rad/s）
  *        p->WeForeLPF      滤波后的电角速度（rad/s）
  *        p->ThetaFore      观测电角度（rad，0~2π）
  *        p->ThetaCompensate 补偿后的电角度（rad，0~2π）
  *        p->ETheta         查表用角度（0~4095，对应 0~2π）
  *
  * @note
  * 1. 本 PLL 基于正交误差构造相位检测器
  * 2. 输入信号通常来自 SMO 估算的反电动势
  * 3. ThetaCompensate 为推荐使用的最终电角度
  * 4. 当锁相环的输入是SMO的输出的时候，反电动势矢量是对齐q轴，d轴理论为0，真实的反电动势在d轴的投影：d = Ealpha*cosθ + Ebeta*sinθ = 0，现在将真实的反电动势Ealpha和Ebeta投影在PLL输出的角度上，得到的误差ThetaErr
  *
  */
void PLL_Calculate(PLL_STRUCT *p){
    //鉴相器
    p->ThetaErr = p->Dir * (p->CosVal * p->Ain + p->SinVal * p->Bin);
    //环路滤波器
    p->PPart  = p->Kp * p->ThetaErr;
    p->IPart += p->Ki * p->ThetaErr;

    //PID输出为角速度
    p->WeFore = p->PPart + p->IPart;
    p->WeForeLPF = p->WeFore * p->WeForeLPFFactor + p->WeForeLPF * (1 - p->WeForeLPFFactor);
    //压控振荡器
    //对角速度进行离散化积分得到角度
    p->ThetaFore += p->WeFore * p->Ts;

    if(p->ThetaFore > 6.28318f)
    {
        p->ThetaFore -= 6.28318f;  		       //角度归一化  0-2Π
    }
    else if(p->ThetaFore < 0)
    {
        p->ThetaFore += 6.28318f;   	       //角度归一化  0-2Π
    }

    //先加上补偿项Δθ，后对滤波后的角速度离散化积分得到角度，θ_comp = Δθ + θ_est
    //角度增量 = 角速度 × 时间
    p->ThetaCompensate = p->WeForeLPF * p->Ts + p->ThetaFore;  //观测角度补偿。

    if(p->ThetaCompensate > 6.28318f)
    {
        p->ThetaCompensate -= 6.28318f;  		 //角度归一化  0-2Π
    }
    else if(p->ThetaCompensate < 0)
    {
        p->ThetaCompensate += 6.28318f;   	 //角度归一化  0-2Π
    }

}
// Reset functions to clear internal observer/PLL states
void ResetHFI(HFI_STRUCT *p)
{
    if(p == NULL) return;
    p->NSDOut = 0;
    p->NSDFlag = 0;
    p->NSDCount = 0;
    p->NSDSum1 = 0.0f;
    p->NSDSum2 = 0.0f;

    p->Uin = HFI_Uin;

    p->IdRef = 0.0f;
    p->IdHigh = 0.0f;
    p->Dir = 0;
    p->Id = 0.0f;
    p->IdLast = 0.0f;
    p->IdBase = 0.0f;
    p->Iq = 0.0f;
    p->IqLast = 0.0f;
    p->IqBase = 0.0f;
    p->Ialpha = 0.0f;
    p->IalphaLast = 0.0f;
    p->IalphaHigh = 0.0f;
    p->IalphaHighLast = 0.0f;
    p->Ibeta = 0.0f;
    p->IbetaLast = 0.0f;
    p->IbetaHigh = 0.0f;
    p->IbetaHighLast = 0.0f;
    p->IalphaOut = 0.0f;
    p->IbetaOut = 0.0f;
}

void ResetSMO(SMO_STRUCT *p)
{
    if(p == NULL) return;
    p->Ialpha = 0.0f;
    p->Ibeta = 0.0f;

    p->IalphaFore = 0.0f;
    p->IbetaFore = 0.0f;

    // keep Rs/Ld/Gain/Ts as configured
    p->Ualpha = 0.0f;
    p->Ubeta = 0.0f;

    p->EalphaFore = 0.0f;
    p->EalphaForeLPF = 0.0f;
    p->EbetaFore = 0.0f;
    p->EbetaForeLPF = 0.0f;
}

void ResetPLL(PLL_STRUCT *p)
{
    if(p == NULL) return;
    p->Dir = 1;
    p->ThetaErr = 0.0f;
    p->ThetaFore = 0.0f;
    p->ThetaCompensate = 0.0f;
    p->ETheta = 0.0f;

    p->SinVal = 0.0f;
    p->CosVal = 1.0f; // cos(0) = 1

    p->PPart = 0.0f;
    p->IPart = 0.0f;

    p->WeFore = 0.0f;
    p->WeForeLPF = 0.0f;
}
