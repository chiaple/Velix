//
// Created by 18135 on 26-1-17.
//

#include <stdio.h>
#include "global.h"

#include "adc.h"
#include "key.h"
#include "key_actions.h"
#include "serial.h"
#include "FOC.h"
#include "sensoruse.h"
#include "velocity.h"
#include "config.h"
#include "sensorless.h"
#include "motor_stop.h"
#include "motor_tone.h"
#include "vofa_profile.h"
#include "tim.h"

volatile uint32_t sys_tick_ms = 0;          // 每1ms +1
volatile uint32_t task_counter = 0;         // 用于分频
static volatile uint8_t vofa_send_flag = 0;

extern DMA_HandleTypeDef hdma_usart3_rx;


MotorSystem Mt;


char ad[20];
/**
  *
  *  系统初始化 （HAL）
  *
  */
void System_Init(void){

    HAL_Delay(200);

    HAL_GPIO_WritePin(SP3485_GPIO_Port,SP3485_Pin,GPIO_PIN_RESET);//高电平发送低电平接收
    /**按键初始化*/
    Key_Init(&key_1,key1_GPIO_Port,key1_Pin);
    Key_Init(&key_2,key2_GPIO_Port,key2_Pin);

    // USART3 DMA 接收指令
    if (UART_DMA_Receive_Init(Serial_GetCommUart(), serial_dma_rx_buf, RX_BUF_SIZE) != HAL_OK) {
        Error_Handler();
    }
    VOFA_Init();

    MotorCommand_Init(&Mt.cmd);//上电状态

    /**编码器初始化*/
    Mt.encoder.EncoderValMax = ENCODER_MAX;

    Mt.foc.U_bus = FOC_UBUS_DEFAULT;//母线电压，可以优化为adc采样再用分压电阻计算
    Mt.sample.BusReal = FOC_UBUS_DEFAULT;
    Mt.foc.IdLPFFactor = FOC_ID_LPF_FACTOR_DEFAULT;
    Mt.foc.IqLPFFactor = FOC_IQ_LPF_FACTOR_DEFAULT;
    Mt.velocity.SpeedCalculateCnt = 0;
    Mt.encoder.PosCalculateCnt = 0;

    Mt.identify.CurMax = IDENTIFY_CUR_MAX_DEFAULT;                       //设置电阻电感识别时的最大母线电流（单位：安）

    Mt.SMO.Gain = SMO_GAIN_DEFAULT;                             //设置滑膜观测器增益
    Mt.SMO.Ts = SMO_TS_DEFAULT;                                  //设置滑膜观测器运行时间间隔
    Mt.SMO.EabForeLPFFactor = SMO_EAB_LPF_FACTOR_DEFAULT;                  //设置预估反电动势低通滤波系数

    Mt.SPLL.Ts = SPLL_TS_DEFAULT;                                 //设置锁相环运行时间间隔
    Mt.SPLL.Kp = SPLL_KP_DEFAULT;                              //设置锁相环比例系数
    Mt.SPLL.Ki = SPLL_KI_DEFAULT;                               //设置锁相环积分系数
    Mt.SPLL.WeForeLPFFactor = SPLL_WE_LPF_FACTOR_DEFAULT;	               //设置观测电角速度低通滤波系数

    Mt.HFI.Uin = HFI_Uin;	                             //设置高频注入的电压幅值
    Mt.HPLL.Dir = HPLL_DIR_DEFAULT;                                 //设置锁相环输入方向
    Mt.HPLL.Kp = HPLL_KP_DEFAULT;                             //设置锁相环比例系数
    Mt.HPLL.Ki = HPLL_KI_DEFAULT;                              //设置锁相环积分系数
    Mt.HPLL.Ts = HPLL_TS_DEFAULT;                                 //设置锁相环运行时间间隔
    Mt.HPLL.WeForeLPFFactor = HPLL_WE_LPF_FACTOR_DEFAULT;                 //设置观测电角速度低通滤波系数

    motor_Init(&Mt.motor, MOTOR_NUMBER_DEFAULT, MOTOR_DIR_DEFAULT, MOTOR_POLE_PAIRS_DEFAULT);

    Mt.HfiSmoSwitch.HfiOnlySpeedLimit = Mt.motor.polePairs * 800.0f;
    Mt.HfiSmoSwitch.HfiDisableSpeedMargin = Mt.motor.polePairs * 900.0f;
    Mt.HfiSmoSwitch.TransitionHighSpeedLimit = Mt.motor.polePairs * 1000.0f;

    /**PID初始化*/
    PID_init(&Mt.pos_pid,
             PID_POS_KP_DEFAULT, PID_POS_KI_DEFAULT, PID_POS_KD_DEFAULT,
             PID_POS_OUT_MIN_DEFAULT, PID_POS_OUT_MAX_DEFAULT,
             PID_POS_I_MIN_DEFAULT, PID_POS_I_MAX_DEFAULT);     //位置环PID输出限幅是最大速度(RPM)
    PID_init(&Mt.HFI_pos_pid,
             PID_HFI_POS_KP_DEFAULT, PID_HFI_POS_KI_DEFAULT, PID_HFI_POS_KD_DEFAULT,
             PID_HFI_POS_OUT_MIN_DEFAULT, PID_HFI_POS_OUT_MAX_DEFAULT,
             PID_HFI_POS_I_MIN_DEFAULT, PID_HFI_POS_I_MAX_DEFAULT); //HFI角度环PID输出限幅是最大速度(RPM)
    PID_init(&Mt.speed_pid,
             PID_SPEED_KP_DEFAULT, PID_SPEED_KI_DEFAULT, PID_SPEED_KD_DEFAULT,
             PID_SPEED_OUT_MIN_DEFAULT, PID_SPEED_OUT_MAX_DEFAULT,
             PID_SPEED_I_MIN_DEFAULT, PID_SPEED_I_MAX_DEFAULT);    //速度环PID输出限幅是最大力矩
    PID_init(&Mt.speed_low_pid,
             PID_SPEED_LOW_KP_DEFAULT, PID_SPEED_LOW_KI_DEFAULT, PID_SPEED_LOW_KD_DEFAULT,
             PID_SPEED_LOW_OUT_MIN_DEFAULT, PID_SPEED_LOW_OUT_MAX_DEFAULT,
             PID_SPEED_LOW_I_MIN_DEFAULT, PID_SPEED_LOW_I_MAX_DEFAULT);
    PID_init(&Mt.iq_pid,
             PID_IQ_KP_DEFAULT, PID_IQ_KI_DEFAULT, PID_IQ_KD_DEFAULT,
             PID_IQ_OUT_MIN_DEFAULT, PID_IQ_OUT_MAX_DEFAULT,
             PID_IQ_I_MIN_DEFAULT, PID_IQ_I_MAX_DEFAULT);       //q轴

    PID_init(&Mt.HFI_speed_pid,
             PID_HFI_SPEED_KP_DEFAULT, PID_HFI_SPEED_KI_DEFAULT, PID_HFI_SPEED_KD_DEFAULT,
             PID_HFI_SPEED_OUT_MIN_DEFAULT, PID_HFI_SPEED_OUT_MAX_DEFAULT,
             PID_HFI_SPEED_I_MIN_DEFAULT, PID_HFI_SPEED_I_MAX_DEFAULT);       //d轴

    PID_init(&Mt.id_pid,
             PID_ID_KP_DEFAULT, PID_ID_KI_DEFAULT, PID_ID_KD_DEFAULT,
             PID_ID_OUT_MIN_DEFAULT, PID_ID_OUT_MAX_DEFAULT,
             PID_ID_I_MIN_DEFAULT, PID_ID_I_MAX_DEFAULT);       //d轴
    PID_init(&Mt.HFI_id_pid,
             PID_HFI_ID_KP_DEFAULT, PID_HFI_ID_KI_DEFAULT, PID_HFI_ID_KD_DEFAULT,
             PID_HFI_ID_OUT_MIN_DEFAULT, PID_HFI_ID_OUT_MAX_DEFAULT,
             PID_HFI_ID_I_MIN_DEFAULT, PID_HFI_ID_I_MAX_DEFAULT);

    //电流校准
    CalibrateCurrentOffset(&Mt.sample);

    //次要任务
    HAL_TIM_Base_Start_IT(&htim2);

    // 开启注入转换和中断，由 HAL callback 驱动 FOC 快环
    HAL_ADCEx_InjectedStart_IT(&hadc1);
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_4, 4000);//adc注入组通道中断触发开关
    //LED初始化
    LED_Init();
    // 电机发声初始化
    MotorTone_Init();
}


void System_Loop(void){

    //串口指令处理
    Serial_ParseCommand();

    //按鍵处理
    Key_Handler(&key_1, SingleClickAction, DoubleClickAction, LongPressAction);
#if COMM_UART_PORT == 3
    if (vofa_send_flag != 0U) {
        vofa_send_flag = 0U;
        VOFA_SendByMode(&Mt);
    }
#endif

}
void FOC_Loop(MotorSystem *p){
    //相电流计算
    Calculate_Phase_Current(&p->sample, &p->foc);

    switch (p->cmd.state) {
        case MOTOR_IDENTIFY:
            Motor_Identify(p);
            break;
        case MOTOR_SENSORUSE:
            Sensoruse_Control(p);
            break;
        case MOTOR_SENSORLESS:
            Sensorless_Control(p);
            break;
        case MOTOR_STOP:
            Motor_StopReset(p);
            break;
    }
    // 电机发声
    MotorTone_Update(p);

    setSVPWM(0, p->foc.Uq, p->foc.Ud, p->encoder.elecAngleRad);

}

void HAL_ADCEx_InjectedConvCpltCallback(ADC_HandleTypeDef *hadc)
{
    if (hadc->Instance == ADC1)
    {
        FOC_Loop(&Mt);
    }
}

// 当 定时器中断回调
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM2)
    {
        sys_tick_ms++;
        task_counter++;

        // 执行任务
        UpdateTransitionTarget(&Mt.cmd.speed, Mt.cmd.fSpeed, CMD_SPEED_TRANSITION_RATE_DEFAULT);
        UpdateTransitionTarget(&Mt.cmd.position, Mt.cmd.fPosition, CMD_POSITION_TRANSITION_RATE_DEFAULT);
        static uint8_t log_cnt = 0U;
        log_cnt++;
        if(log_cnt >= VOFA_SEND_DIV_DEFAULT){
            log_cnt = 0U;
            vofa_send_flag = 1U;
        }
        LED_Tick_1ms();

        if ((task_counter % 10) == 0) {
            Key_Scan(&key_1);
        }
        // ... 其他任务同理
    }
}
//1
//2
