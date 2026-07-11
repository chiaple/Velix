//
// Created by e on 26-1-20.
//

#ifndef SPIN_MASTER_V2_CONFIG_H
#define SPIN_MASTER_V2_CONFIG_H

#include "arm_math.h"
#include "main.h"
#include "adc.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "stm32g4xx_ll_gpio.h"

//=============================Board mapping==============================
// Keep CubeMX-generated names here. USER modules should prefer VELIX_* names
// so board migration only needs this section adjusted.
#define VELIX_RS485_DE_GPIO_Port        SP3485_GPIO_Port
#define VELIX_RS485_DE_Pin              SP3485_Pin

#define VELIX_KEY1_GPIO_Port            key1_GPIO_Port
#define VELIX_KEY1_Pin                  key1_Pin
#define VELIX_KEY2_GPIO_Port            key2_GPIO_Port
#define VELIX_KEY2_Pin                  key2_Pin

#define VELIX_LED_R_GPIO_Port           LED_R_GPIO_Port
#define VELIX_LED_R_Pin                 LED_R_Pin
#define VELIX_LED_G_GPIO_Port           LED_G_GPIO_Port
#define VELIX_LED_G_Pin                 LED_G_Pin
#define VELIX_LED_B_GPIO_Port           LED_B_GPIO_Port
#define VELIX_LED_B_Pin                 LED_B_Pin

#define VELIX_ENCODER_SPI               hspi1
#define VELIX_ENCODER_CS_GPIO_Port      Encoder_CS_GPIO_Port
#define VELIX_ENCODER_CS_Pin            Encoder_CS_Pin

#define VELIX_ROTARY_TIM                htim4
#define VELIX_ROTARY_BUTTON_GPIO_Port   encoder_GPIO_Port
#define VELIX_ROTARY_BUTTON_Pin         encoder_Pin

#define VELIX_PWM_TIM                   htim1
#define VELIX_PWM_U_CHANNEL             TIM_CHANNEL_1
#define VELIX_PWM_V_CHANNEL             TIM_CHANNEL_2
#define VELIX_PWM_W_CHANNEL             TIM_CHANNEL_3
#define VELIX_PWM_ADC_TRIG_CHANNEL      TIM_CHANNEL_4
#define VELIX_TASK_TIM                  htim2
#define VELIX_ADC_HANDLE                hadc1
#define VELIX_ADC_INSTANCE              ADC1
#define VELIX_ADC_IU_RANK               ADC_INJECTED_RANK_1
#define VELIX_ADC_IV_RANK               ADC_INJECTED_RANK_2

#define VELIX_COMM_UART_PORT            3
#define VELIX_COMM_UART_HANDLE          huart3

typedef HAL_StatusTypeDef Velix_Status;
typedef UART_HandleTypeDef Velix_UartHandle;
typedef GPIO_TypeDef Velix_GpioPort;
typedef ADC_HandleTypeDef Velix_AdcHandle;
typedef TIM_HandleTypeDef Velix_TimerHandle;

#define VELIX_OK                        HAL_OK
#define VELIX_ERROR                     HAL_ERROR
#define VELIX_BUSY                      HAL_BUSY

#define VELIX_ADC_INJECTED_CALLBACK     HAL_ADCEx_InjectedConvCpltCallback
#define VELIX_TIMER_PERIOD_CALLBACK     HAL_TIM_PeriodElapsedCallback

#define VELIX_RS485_RX_MODE() \
    HAL_GPIO_WritePin(VELIX_RS485_DE_GPIO_Port, VELIX_RS485_DE_Pin, GPIO_PIN_RESET)

#define VELIX_RS485_TX_MODE() \
    HAL_GPIO_WritePin(VELIX_RS485_DE_GPIO_Port, VELIX_RS485_DE_Pin, GPIO_PIN_SET)

#define VELIX_ENCODER_CS_LOW() \
    HAL_GPIO_WritePin(VELIX_ENCODER_CS_GPIO_Port, VELIX_ENCODER_CS_Pin, GPIO_PIN_RESET)

#define VELIX_ENCODER_CS_HIGH() \
    HAL_GPIO_WritePin(VELIX_ENCODER_CS_GPIO_Port, VELIX_ENCODER_CS_Pin, GPIO_PIN_SET)

#define VELIX_LED_ON(port, pin) \
    HAL_GPIO_WritePin((port), (pin), GPIO_PIN_RESET)

#define VELIX_LED_OFF(port, pin) \
    HAL_GPIO_WritePin((port), (pin), GPIO_PIN_SET)

#define VELIX_GPIO_IS_SET(port, pin) \
    LL_GPIO_IsInputPinSet((port), (pin))

#define Velix_RotaryStart() \
    HAL_TIM_Encoder_Start(&VELIX_ROTARY_TIM, TIM_CHANNEL_ALL)

#define Velix_RotaryGetCounter() \
    __HAL_TIM_GET_COUNTER(&VELIX_ROTARY_TIM)

#define Velix_RotarySetCounter(value) \
    __HAL_TIM_SET_COUNTER(&VELIX_ROTARY_TIM, (value))

#define VELIX_PWM_SET_U(compare) \
    __HAL_TIM_SET_COMPARE(&VELIX_PWM_TIM, VELIX_PWM_U_CHANNEL, (compare))

#define VELIX_PWM_SET_V(compare) \
    __HAL_TIM_SET_COMPARE(&VELIX_PWM_TIM, VELIX_PWM_V_CHANNEL, (compare))

#define VELIX_PWM_SET_W(compare) \
    __HAL_TIM_SET_COMPARE(&VELIX_PWM_TIM, VELIX_PWM_W_CHANNEL, (compare))

#define VELIX_PWM_SET_ADC_TRIGGER(compare) \
    __HAL_TIM_SET_COMPARE(&VELIX_PWM_TIM, VELIX_PWM_ADC_TRIG_CHANNEL, (compare))

#define VELIX_ADC_START_REG_CONVERSION() \
    HAL_ADC_Start(&VELIX_ADC_HANDLE)

#define VELIX_ADC_REG_EOC() \
    (HAL_ADC_PollForConversion(&VELIX_ADC_HANDLE, 10) == HAL_OK)

#define VELIX_ADC_READ_REG12() \
    HAL_ADC_GetValue(&VELIX_ADC_HANDLE)

#define VELIX_ADC_STOP_REG_CONVERSION() \
    HAL_ADC_Stop(&VELIX_ADC_HANDLE)

#define VELIX_ADC_START_REG_DMA(buffer, count) \
    HAL_ADC_Start_DMA(&VELIX_ADC_HANDLE, (uint32_t *)(buffer), (count))

#define VELIX_ADC_DISABLE_REG_DMA_XFER_IRQ() \
    __HAL_DMA_DISABLE_IT(VELIX_ADC_HANDLE.DMA_Handle, DMA_IT_HT | DMA_IT_TC)

#define VELIX_ADC_READ_INJ12(rank) \
    HAL_ADCEx_InjectedGetValue(&VELIX_ADC_HANDLE, (rank))

#define VELIX_ADC_START_INJ_CONVERSION() \
    HAL_ADCEx_InjectedStart(&VELIX_ADC_HANDLE)

#define VELIX_ADC_INJ_EOC(timeout_ms) \
    (HAL_ADCEx_InjectedPollForConversion(&VELIX_ADC_HANDLE, (timeout_ms)) == HAL_OK)

#define VELIX_ADC_STOP_INJ_CONVERSION() \
    HAL_ADCEx_InjectedStop(&VELIX_ADC_HANDLE)

#define Velix_DelayMs(ms) \
    HAL_Delay((ms))

#define Velix_ErrorHandler() \
    Error_Handler()

#define Velix_TaskTimerStartInterrupt() \
    HAL_TIM_Base_Start_IT(&VELIX_TASK_TIM)

#define Velix_IsTaskTimer(timer) \
    ((timer)->Instance == VELIX_TASK_TIM.Instance)

#define Velix_AdcInjectedStartInterrupt() \
    HAL_ADCEx_InjectedStart_IT(&VELIX_ADC_HANDLE)

#define Velix_IsCurrentAdc(adc) \
    ((adc)->Instance == VELIX_ADC_INSTANCE)

#define Velix_EncoderTransfer16(tx_data, rx_data, timeout_ms) \
    HAL_SPI_TransmitReceive(&VELIX_ENCODER_SPI, \
                            (uint8_t *)&(tx_data), \
                            (uint8_t *)(rx_data), \
                            1, \
                            (timeout_ms))

#define Velix_UartCommHandle() \
    (&VELIX_COMM_UART_HANDLE)

#define Velix_UartIsIdle(uart) \
    (__HAL_UART_GET_FLAG((uart), UART_FLAG_IDLE) != RESET)

#define Velix_UartClearIdle(uart) \
    __HAL_UART_CLEAR_IDLEFLAG((uart))

#define Velix_UartDmaRemaining(uart) \
    __HAL_DMA_GET_COUNTER((uart)->hdmarx)

#define Velix_UartStartDmaRx(uart, buffer, bytes) \
    HAL_UART_Receive_DMA((uart), (buffer), (bytes))

#define Velix_UartEnableIdleIrq(uart) \
    __HAL_UART_ENABLE_IT((uart), UART_IT_IDLE)

#define Velix_UartEnable(uart) \
    __HAL_UART_ENABLE((uart))

#define Velix_UartIsReady(uart) \
    ((uart)->gState == HAL_UART_STATE_READY)

#define Velix_UartStartDmaTx(uart, data, bytes) \
    HAL_UART_Transmit_DMA((uart), (uint8_t *)(data), (bytes))

#define   Udc                               30.0f               //母线电压
#define _1_SQRT3 		                    0.57735027f         // 1/√3
#define VELIX_PWM_PERIOD_TICKS              4249

#define TS                                  0.00005f //20kHz FOC执行间隔

#define SAMPLING_RESITOR                    0.005f      //相电流采样电阻
#define MAGNIFICATION                       10.0f       //放大器倍数
#define ADC_RESOLUTION                      4096.0f     //ADC分辨率
#define ADC_VREF                            3.3f        //ADC基准电压

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
#define VOFA_SEND_DIV_DEFAULT               10U         // VOFA 发送分频，每 N 次 FOC 快环发送一次
#define ROTARY_COUNTS_PER_REV_DEFAULT       60.0f       // 旋钮转一整圈对应的编码器计数
#define ROTARY_SPEED_PER_REV_DEFAULT        1000.0f      // 旋钮转一整圈对应的目标速度变化 RPM
#define ROTARY_DIRECTION_DEFAULT            1           // 旋钮方向，1 保持当前方向，-1 反向
#define ROTARY_SPEED_MIN_DEFAULT            (-4000.0f)  // 旋钮速度目标下限 RPM
#define ROTARY_SPEED_MAX_DEFAULT            4000.0f     // 旋钮速度目标上限 RPM

//=============================无感/HFI默认参数==============================
#define SENSORLESS_STRONG_DRAG_UD_DEFAULT               2.0f        // 开环强拖时施加的 d 轴电压
#define SENSORLESS_STRONG_DRAG_CURRENT_D_DEFAULT        2.0f        // 电流闭环强拖时 d 轴给定
#define SENSORLESS_STRONG_DRAG_ELEC_RPM_DEFAULT         100        // 强拖电角速度设定 RPM
#define SENSORLESS_STRONG_DRAG_MECH_RPM_DEFAULT         300         // 强拖机械速度设定 RPM
#define SENSORLESS_STRONG_DRAG_ALIGN_CURRENT_D_DEFAULT  6.0f        // 滑膜切换前 d 轴强拖电流
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
#define COMM_UART_PORT VELIX_COMM_UART_PORT
#define RX_BUF_SIZE 512
// VOFA JustFloat: 每个通道 4 字节 float，帧尾固定 4 字节
#define VOFA_MAX_CH_COUNT 16
#define VOFA_FRAME_BYTES(count)  ((count) * 4U + 4U)

#define HFI_IdRef   3.0f
#define HFI_Uin     2.0f
#endif //SPIN_MASTER_V2_CONFIG_H

//test
//test2
