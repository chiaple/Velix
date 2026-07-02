//
// Created by e on 26-1-20.
//

#ifndef SPIN_MASTER_V2_CONFIG_H
#define SPIN_MASTER_V2_CONFIG_H

#include "arm_math.h"

#define   Udc                               20.0f               //母线电压
#define _1_SQRT3 		                    0.57735027f         // 1/√3
#define TIM1_ARR                            4249

#define TS                                  0.00005f //20kHz FOC执行间隔

#define SAMPLING_RESITOR                    0.005f      //相电流采样电阻
#define MAGNIFICATION                       20.0f       //分流放大器倍数
#define ADC_RESOLUTION                      4096.0f     //ADC分辨率
#define ADC_VREF                            3.25f        //ADC基准电压

#define ENCODER_MAX                         16384.0f    //外部磁编码器最大值

//=============================默认运行参数==============================
#define FOC_UBUS_DEFAULT                    Udc
#define FOC_ID_LPF_FACTOR_DEFAULT           0.1f
#define FOC_IQ_LPF_FACTOR_DEFAULT           0.1f

#define IDENTIFY_CUR_MAX_DEFAULT            0.6f

#define SMO_GAIN_DEFAULT                    14.0f
#define SMO_TS_DEFAULT                      TS
#define SMO_EAB_LPF_FACTOR_DEFAULT          0.1f

#define SPLL_TS_DEFAULT                     TS
#define SPLL_KP_DEFAULT                     80.0f
#define SPLL_KI_DEFAULT                     0.5f
#define SPLL_WE_LPF_FACTOR_DEFAULT          0.01f

#define HPLL_DIR_DEFAULT                    1
#define HPLL_TS_DEFAULT                     TS
#define HPLL_KP_DEFAULT                     1000.0f
#define HPLL_KI_DEFAULT                     20.0f
#define HPLL_WE_LPF_FACTOR_DEFAULT          0.01f

#define MOTOR_NUMBER_DEFAULT                0
#define MOTOR_DIR_DEFAULT                   1
#define MOTOR_POLE_PAIRS_DEFAULT            7

#define ENCODER_CALIB_UD_DEFAULT            1.0f        // 有感校准时施加的 d 轴电压
#define ENCODER_CALIB_TIME_MS_DEFAULT       1000U       // 有感校准保持时间

#define CMD_SPEED_TRANSITION_RATE_DEFAULT   0.005f      // 速度目标渐变系数
#define CMD_POSITION_TRANSITION_RATE_DEFAULT 0.01f       // 位置目标渐变系数
#define VOFA_SEND_DIV_DEFAULT               2U          // VOFA 发送分频，每 N 次 FOC 发送一次

//=============================无感/HFI默认参数==============================
#define SENSORLESS_STRONG_DRAG_UD_DEFAULT               2.0f        // 开环强拖时施加的 d 轴电压
#define SENSORLESS_STRONG_DRAG_CURRENT_D_DEFAULT        2.0f        // 电流闭环强拖时 d 轴给定
#define SENSORLESS_STRONG_DRAG_ELEC_RPM_DEFAULT         2000        // 强拖电角速度设定 RPM
#define SENSORLESS_STRONG_DRAG_MECH_RPM_DEFAULT         300         // 强拖机械速度设定 RPM
#define SENSORLESS_STRONG_DRAG_ALIGN_CURRENT_D_DEFAULT  4.0f        // 滑膜切换前 d 轴强拖电流
#define SENSORLESS_SWITCH_MECH_RPM_BASE_DEFAULT         1000        // 强拖切观测器速度门限基准值，实际按极对数换算
#define SENSORLESS_COMPARE_ANGLE_MIN_RAD_DEFAULT        0.767f      // 观测角比较有效区间下限
#define SENSORLESS_COMPARE_ANGLE_MAX_RAD_DEFAULT        5.370f      // 观测角比较有效区间上限
#define SENSORLESS_COMPARE_ERROR_RATIO_DEFAULT          0.1f        // 观测角允许误差占整周期比例
#define SENSORLESS_COMPARE_COUNT_DEFAULT                300U        // 观测角比较次数门限
#define SENSORLESS_COMPARE_SUCCESS_RATIO_DEFAULT        0.9f        // 判定切换成功所需相似比例
#define SENSORLESS_STARTUP_TIMEOUT_DEFAULT              40000U      // 无感启动超时计数
#define SENSORLESS_SPEED_LOOP_DIV_DEFAULT               2U          // 无感速度环分频
#define SENSORLESS_MIN_RUNNING_RPM_DEFAULT              800         // 滑膜运行最小机械速度
#define SENSORLESS_BLOCK_CHECK_DELAY_DEFAULT            20000U      // 进入堵转判断前的运行计数
#define SENSORLESS_BLOCK_CHECK_RPM_BASE_DEFAULT         1000        // 堵转判断机械速度基准值

#define HFI_SPEED_LOOP_DIV_DEFAULT                      2U          // HFI速度环分频
//=============================PID默认参数==============================
#define PID_POS_KP_DEFAULT                  1.9f        // 位置环比例系数
#define PID_POS_KI_DEFAULT                  0.0f        // 位置环积分系数
#define PID_POS_KD_DEFAULT                  0.0f        // 位置环微分系数
#define PID_POS_OUT_MIN_DEFAULT             -250.0f     // 位置环输出下限，对应目标速度下限
#define PID_POS_OUT_MAX_DEFAULT             250.0f      // 位置环输出上限，对应目标速度上限
#define PID_POS_I_MIN_DEFAULT               -0.0f       // 位置环积分下限
#define PID_POS_I_MAX_DEFAULT               0.0f        // 位置环积分上限

#define PID_HFI_POS_KP_DEFAULT              30.0f        // HFI角度环比例系数
#define PID_HFI_POS_KI_DEFAULT              0.0f        // HFI角度环积分系数
#define PID_HFI_POS_KD_DEFAULT              0.0f        // HFI角度环微分系数
#define PID_HFI_POS_OUT_MIN_DEFAULT         -550.0f     // HFI角度环输出下限，对应目标速度下限
#define PID_HFI_POS_OUT_MAX_DEFAULT         550.0f      // HFI角度环输出上限，对应目标速度上限
#define PID_HFI_POS_I_MIN_DEFAULT           -0.0f       // HFI角度环积分下限
#define PID_HFI_POS_I_MAX_DEFAULT           0.0f        // HFI角度环积分上限

#define PID_SPEED_KP_DEFAULT                0.001f      // 速度环比例系数
#define PID_SPEED_KI_DEFAULT                0.0002f     // 速度环积分系数
#define PID_SPEED_KD_DEFAULT                0.0f        // 速度环微分系数
#define PID_SPEED_OUT_MIN_DEFAULT           -20.0f      // 速度环输出下限，对应目标力矩/电流下限
#define PID_SPEED_OUT_MAX_DEFAULT           20.0f       // 速度环输出上限，对应目标力矩/电流上限
#define PID_SPEED_I_MIN_DEFAULT             -150000.0f  // 速度环积分下限
#define PID_SPEED_I_MAX_DEFAULT             150000.0f   // 速度环积分上限

#define PID_SPEED_LOW_KP_DEFAULT            0.02f       // 低速速度环比例系数（位置模式内环）
#define PID_SPEED_LOW_KI_DEFAULT            0.001f      // 低速速度环积分系数（位置模式内环）
#define PID_SPEED_LOW_KD_DEFAULT            0.0f        // 低速速度环微分系数（位置模式内环）
#define PID_SPEED_LOW_OUT_MIN_DEFAULT       -10.0f      // 低速速度环输出下限
#define PID_SPEED_LOW_OUT_MAX_DEFAULT       10.0f       // 低速速度环输出上限
#define PID_SPEED_LOW_I_MIN_DEFAULT         -10000.0f   // 低速速度环积分下限
#define PID_SPEED_LOW_I_MAX_DEFAULT         10000.0f    // 低速速度环积分上限

#define PID_IQ_KP_DEFAULT                   2.8f        // q轴电流环比例系数
#define PID_IQ_KI_DEFAULT                   0.5f        // q轴电流环积分系数
#define PID_IQ_KD_DEFAULT                   0.0f        // q轴电流环微分系数
#define PID_IQ_OUT_MIN_DEFAULT              -11.5f      // q轴电流环输出下限，对应Uq下限
#define PID_IQ_OUT_MAX_DEFAULT              11.5f       // q轴电流环输出上限，对应Uq上限
#define PID_IQ_I_MIN_DEFAULT                -4.0f       // q轴电流环积分下限
#define PID_IQ_I_MAX_DEFAULT                4.0f        // q轴电流环积分上限

#define PID_ID_KP_DEFAULT                   0.1f        // d轴电流环比例系数
#define PID_ID_KI_DEFAULT                   1.0f        // d轴电流环积分系数
#define PID_ID_KD_DEFAULT                   0.0f        // d轴电流环微分系数
#define PID_ID_OUT_MIN_DEFAULT              -2.5f       // d轴电流环输出下限，对应Ud下限
#define PID_ID_OUT_MAX_DEFAULT              2.5f        // d轴电流环输出上限，对应Ud上限
#define PID_ID_I_MIN_DEFAULT                -2.0f       // d轴电流环积分下限
#define PID_ID_I_MAX_DEFAULT                2.0f        // d轴电流环积分上限

#define PID_HFI_SPEED_KP_DEFAULT            0.002f      // HFI速度环比例系数
#define PID_HFI_SPEED_KI_DEFAULT            0.0001f     // HFI速度环积分系数
#define PID_HFI_SPEED_KD_DEFAULT            0.0f        // HFI速度环微分系数
#define PID_HFI_SPEED_OUT_MIN_DEFAULT       -3.0f       // HFI速度环输出下限
#define PID_HFI_SPEED_OUT_MAX_DEFAULT       3.0f        // HFI速度环输出上限
#define PID_HFI_SPEED_I_MIN_DEFAULT         -100000.0f  // HFI速度环积分下限
#define PID_HFI_SPEED_I_MAX_DEFAULT         100000.0f   // HFI速度环积分上限

#define PID_HFI_ID_KP_DEFAULT               0.05f       // HFI d轴电流环比例系数
#define PID_HFI_ID_KI_DEFAULT               0.5f        // HFI d轴电流环积分系数
#define PID_HFI_ID_KD_DEFAULT               0.0f        // HFI d轴电流环微分系数
#define PID_HFI_ID_OUT_MIN_DEFAULT          -2.5f       // HFI d轴电流环输出下限
#define PID_HFI_ID_OUT_MAX_DEFAULT          2.5f        // HFI d轴电流环输出上限
#define PID_HFI_ID_I_MIN_DEFAULT            -2.0f       // HFI d轴电流环积分下限
#define PID_HFI_ID_I_MAX_DEFAULT            2.0f        // HFI d轴电流环积分上限

//=============================串口==============================
#define COMM_UART_PORT 3
#define RX_BUF_SIZE 512
// VOFA JustFloat: 每个通道 4 字节 float，帧尾固定 4 字节
#define VOFA_MAX_CH_COUNT 16
#define VOFA_FRAME_BYTES(count)  ((count) * 4U + 4U)

#define HFI_IdRef   3.0f
#define HFI_Uin     2.0f
#endif //SPIN_MASTER_V2_CONFIG_H

//test
//test2
