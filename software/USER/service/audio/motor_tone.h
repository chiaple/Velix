#ifndef SPIN_MASTER_V2_MOTOR_TONE_H
#define SPIN_MASTER_V2_MOTOR_TONE_H

#include "global.h"

#define C4 261.63f
#define D4 293.66f
#define E4 329.63f
#define F4 349.23f
#define G4 392.00f
#define A4 440.00f
#define B4 493.88f
#define C5 523.25f
#define D5 587.33f
#define E5 659.25f
#define F5 698.46f
#define G5 783.99f
#define A5 880.00f
#define B5 987.77f
#define C6 1046.50f
#define D6 1174.66f
#define E6 1318.51f
#define F6 1396.91f
#define G6 1567.98f
#define A6 1760.00f
#define B6 1975.53f
#define FS4 369.99f
#define GS4 415.30f
#define CS5 554.37f
#define CS4 277.18f



void MotorTone_Init(void);
void MotorTone_Play(float freq, uint32_t duration_ms, float amplitude);
void MotorTone_Stop(void);
void MotorTone_Update(MotorSystem *p);

// 播放一段旋律：传入频率数组和对应时长数组（毫秒），count 为元素数量。
// amplitude 单位为电压（V），请确保在安全范围内（例如 <= Mt.foc.U_bus * 0.5）。
void MotorTone_PlayMelody(const float *freqs, const uint32_t *durations_ms, int count, float amplitude);

// 运行时查询接口（非阻塞，用于主循环诊断）
uint8_t MotorTone_IsPlaying(void);
uint32_t MotorTone_GetUpdateCount(void);

#endif // SPIN_MASTER_V2_MOTOR_TONE_H
