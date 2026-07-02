//
// Created by e on 26-1-20.
//

#include <stdio.h>
#include "identify.h"
#include "global.h"
#include "usart.h"

/**
  * 电机参数识别
 */

char idf[40];

void Motor_Identify(MotorSystem *p){
    switch (p->identify.state){
        case RESISTANCE_IDENTIFICATION:       //电阻识别
        {
            if(p->identify.Flag == 0)           //清空参数
            {
                p->foc.Uq = 0;
                p->foc.Ud = 0;
                p->identify.Count = 0;
                p->identify.WaitTim = 0;
                p->identify.Flag = 1;
            }
            if(p->identify.Flag == 1)
            {
                if((p->foc.iu * p->foc.Ud * 1.5f) / p->sample.BusReal >= 0.6f * p->identify.CurMax)
                {
                    p->identify.Flag = 2;
                }
                else
                {
                    p->foc.Ud += 0.0001f;				   	     //逐渐增加电流（过大电机发热，过小测量不准）
                    p->identify.VoltageSet[0] = p->foc.Ud;
                }
            }
            if(p->identify.Flag == 2)
            {
                p->identify.WaitTim++;
                if(p->identify.WaitTim > 4000)       // 0.2S 等待电流稳定
                {
                    p->identify.CurSum += p->foc.iu;
                }

                if(p->identify.WaitTim >= 4100)      // 记录100次电流值
                {
                    p->identify.CurAverage[0] = p->identify.CurSum * 0.01f; //计算平均电流
                    p->identify.WaitTim = 0;
                    p->identify.CurSum = 0;
                    p->identify.Flag = 3;
                }
            }
            if(p->identify.Flag == 3)
            {
                if((p->foc.iu * p->foc.Ud * 1.5f) / p->sample.BusReal >= p->identify.CurMax)
                {
                    p->identify.Flag = 4;
                }
                else
                {
                    p->foc.Ud += 0.0001f;					    // 逐渐增加电流（过大电机发热，过小测量不准）
                    p->identify.VoltageSet[1] = p->foc.Ud;
                }
            }
            if(p->identify.Flag == 4)
            {
                p->identify.WaitTim++;
                if(p->identify.WaitTim > 4000)       // 0.2S 等待电流稳定
                {
                    p->identify.CurSum += p->foc.iu;
                }
                if(p->identify.WaitTim >= 4100)      // 记录100次电流值
                {
                    p->identify.CurAverage[1] = p->identify.CurSum * 0.01f; //计算平均电流
                    p->identify.WaitTim = 0;
                    p->identify.CurSum = 0;
                    p->identify.Flag = 5;
                }
            }
            if(p->identify.Flag == 5)
            {
                p->identify.Rs = (p->identify.VoltageSet[1] - p->identify.VoltageSet[0]) / (p->identify.CurAverage[1] - p->identify.CurAverage[0]);
                p->foc.Ud = 0;
                p->identify.Flag = 0;
                p->identify.state = INDUCTANCE_IDENTIFICATION;
            }
            p->encoder.elecAngleRad = 0;
        }
        break;
        case INDUCTANCE_IDENTIFICATION:             //电感识别
        {
            if(p->identify.Flag == 0)
            {
                p->foc.Uq = 0;
                p->foc.Ud = 0;
                if(p->foc.iu >= -0.05f && p->foc.iu <= 0.05f)
                {
                    p->identify.Flag = 1;
                }
            }

            if(p->identify.Flag == 1)
            {
                p->foc.Ud = p->identify.VoltageSet[1];
                p->identify.WaitTim++;
                if(p->foc.iu >= p->identify.CurAverage[1] * 0.95f)
                {
                    p->identify.LsSum += p->identify.Rs * 0.334f * 0.00005f *  p->identify.WaitTim;
                    p->identify.WaitTim = 0;
                    p->identify.Count++;
                    p->identify.Flag = 0;
                    p->foc.Ud = 0;
                    if(p->identify.Count >= 100)
                    {
                        p->identify.Flag = 2;
                    }
                }
            }
            if(p->identify.Flag == 2)
            {
                p->identify.Ls = p->identify.LsSum * 0.01f;
                p->identify.Ld = p->identify.Ls;
                p->identify.Lq = p->identify.Ls;

                p->identify.Flag = 0;
                p->identify.LsSum = 0;
                p->identify.WaitTim = 0;
                p->identify.state = IDENTIFY_DONE;
                p->cmd.state = MOTOR_SENSORLESS;
                p->identify.EndFlag = 1;
                //识别后给到SMO
                p->SMO.Rs = p->identify.Rs;
                p->SMO.Ld = p->identify.Ld;
            }
            p->encoder.elecAngleRad = 0;
        }
        break;
    }
}