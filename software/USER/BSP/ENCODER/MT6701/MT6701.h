//
// Created by 18135 on 26-1-17.
//

#ifndef SPIN_MASTER_V2_MT6701_H
#define SPIN_MASTER_V2_MT6701_H
#include "gpio.h"
#include "arm_math.h"


#define MT6701_CS_LOW   HAL_GPIO_WritePin(Encoder_CS_GPIO_Port, Encoder_CS_Pin, GPIO_PIN_RESET)
#define MT6701_CS_HIGH  HAL_GPIO_WritePin(Encoder_CS_GPIO_Port, Encoder_CS_Pin, GPIO_PIN_SET)

/*typedef struct {
    int			motorNum;
    uint16_t 	rawAngle;			// 原始数据：0-4095
    float32_t	degAngle;			// 角度：0-360
    int16_t  	numOfCircle;		// 圈数
    float32_t 	angle;				// 角度：0-2pi
    float32_t 	eAngle;				// 电角度
    //    bool 		result;       		// false:不使用	true:使用
    float32_t 	velocity;			// 速度

    float32_t RPM;

    float32_t	position;			// 位置
    float32_t 	lastAngle;			// 上一个角度
    uint32_t 	lastTs;				// 上一个时间
    float32_t 	lastTotalAngle; 	// 带方向连续性的总角度缓存
    float32_t   curDegAngle; 		// 当前单圈角度（0-360°）
    int32_t  	numOfCircles;		// 累计圈数（正数为顺时针，负数为逆时针）
    float32_t   absDegAngle;		// 绝对角度 = curDegAngle + numOfCircles*360
}MT6701;*/

uint16_t MT6701_ReadRaw(void);


#endif //SPIN_MASTER_V2_MT6701_H
