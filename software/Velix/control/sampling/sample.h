//
// Created by e on 26-1-20.
//

#ifndef SPIN_MASTER_V2_SAMPLE_H
#define SPIN_MASTER_V2_SAMPLE_H
#include "arm_math.h"
#include "FOC.h"
typedef struct
{
    int8_t    CurrentDir;                 // 电流采样方向
    uint8_t    EndFlag;                    // 计算完成标志
    uint16_t   OffsetCnt;                  // 基准值计算次数
    int32_t   BusOffset; 	                // 母线电压基准值
    uint16_t   IuOffset;                   // U相电流偏置值
    uint16_t   IvOffset;                   // V相电流偏置值
    uint16_t  IwOffset;                   // W相电流偏置值
    int32_t  BusRaw; 	                  // 母线电压原始值
    int32_t   IuRaw;                      // U相电流原始值
    int32_t   IvRaw;                      // V相电流原始值
    int32_t   IwRaw;                      // W相电流原始值
    float IuReal;                     // U相电流真实值
    float IvReal;                     // V相电流真实值
    float IwReal;                     // W相电流真实值
    float BusReal;                    // 母线电压真实值(动态)
    float BusCalibReal;               // 母线电压真实值(校准值，静态)
    float BusChange;                  // 母线电压变化值
    float BusFactor;                  // 母线电压计算系数
    float CurrentFactor;              // 相电流计算系数
    uint16_t   AdcBuff[3];                 // 用于ADC规则通道接收数据
}SAMPLE_STRUCT;

void CalibrateCurrentOffset(SAMPLE_STRUCT *sample);
void Calculate_Phase_Current(SAMPLE_STRUCT *sample, FOC_STRUCT *foc);
void Sample_UpdateBusVoltage(SAMPLE_STRUCT *sample);

#endif //SPIN_MASTER_V2_SAMPLE_H
