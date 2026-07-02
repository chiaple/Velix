#include "motor_tone.h"
#include "FOC.h"
#include "config.h"
#include "arm_math.h"
#include "usart.h"
#include "tone_presets.h"

// 一个非常简单的实现：在 q 轴电压 Uq 上叠加一个正弦波，用以驱动电机产生声音。
// 注意：使用电机发声会产生转矩并可能转动电机，必须在安全条件下运行。

static float tone_freq = 0.0f; // Hz
static uint32_t tone_end_ms = 0; // 结束时间（sys_tick_ms）
static float tone_amplitude = 0.0f; // q 轴电压幅值
static uint8_t tone_playing = 0;
static float phase = 0.0f;
static uint32_t update_count = 0; // 被 MotorTone_Update 调用计数，用于诊断

// melody state
static const float *mel_freqs = NULL;
static const uint32_t *mel_durations = NULL;
static int mel_count = 0;
static int mel_index = 0;
static uint8_t mel_active = 0;
void MotorTone_Init(void){
    tone_freq = 0.0f;
    tone_end_ms = 0;
    tone_amplitude = 0.0f;
    tone_playing = 0;
    phase = 0.0f;
    update_count = 0;

    mel_freqs = NULL;
    mel_durations = NULL;
    mel_count = 0;
    mel_index = 0;
    mel_active = 0;
    MotorTone_PlayStartupMelody();
}

void MotorTone_Play(float freq, uint32_t duration_ms, float amplitude){
    if (freq <= 0.0f) return;
    tone_freq = freq;
    tone_amplitude = amplitude;
    tone_end_ms = sys_tick_ms + duration_ms;
    tone_playing = 1;
    phase = 0.0f;

}

void MotorTone_Stop(void){
    tone_playing = 0;
    tone_freq = 0.0f;
    tone_amplitude = 0.0f;
    mel_active = 0;
}

void MotorTone_PlayMelody(const float *freqs, const uint32_t *durations_ms, int count, float amplitude){
    if (freqs == NULL || durations_ms == NULL || count <= 0) return;
    mel_freqs = freqs;
    mel_durations = durations_ms;
    mel_count = count;
    mel_index = 0;
    mel_active = 1;
    // start first note
    MotorTone_Play(mel_freqs[0], mel_durations[0], amplitude);
}

void MotorTone_Update(MotorSystem *p){
    // 计数（供主循环读取，注意在中断中写入后主循环读取是安全的）
    update_count++;

    if (mel_active) {
        // 如果当前通道不在播放，但 melody 仍标记 active，启动下一个音
        if (!tone_playing && mel_index < mel_count - 1) {
            mel_index++;
            MotorTone_Play(mel_freqs[mel_index], mel_durations[mel_index], tone_amplitude == 0.0f ? 1.0f : tone_amplitude);
        } else if (!tone_playing && mel_index >= mel_count - 1) {
            // melody 结束
            mel_active = 0;
            MotorTone_Stop();
        }
    }

    if (!tone_playing) return;

    if (sys_tick_ms >= tone_end_ms){
        // 当前音结束，停止播放（melody handler 会在下一次 Update 启动下一个音）
        tone_playing = 0;
        return;
    }

    // 以 FOC 调用间隔 TS 为步长推进相位
    phase += 2.0f * 3.14159265358979323846f * tone_freq * TS; // TS 在 config.h 中定义
    if (phase > 2.0f * 3.14159265358979323846f) phase -= 2.0f * 3.14159265358979323846f;

    // 生成正弦波并叠加到 q 轴电压上
    float tone_uq = tone_amplitude * arm_sin_f32(phase);

    // 简单地把 tone_uq 直接设置为 p->foc.Uq（或叠加），这里我们叠加到现有控制量
    p->foc.Uq += tone_uq;
}

uint8_t MotorTone_IsPlaying(void){
    return tone_playing || mel_active;
}

uint32_t MotorTone_GetUpdateCount(void){
    return update_count;
}
