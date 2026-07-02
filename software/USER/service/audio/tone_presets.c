#include "tone_presets.h"
#include "motor_tone.h"

static const float startup_freqs[] = {
        1800.0f,
        2200.0f,
        2600.0f,
        3000.0f
};

static const uint32_t startup_durs[] = {
        120, 120, 120, 250
};

static const float song_freqs[] = {
        C5, C5, G5, G5, A5, A5, G5,
        F5, F5, E5, E5, D5, D5, C5
};

static const uint32_t song_durations[] = {
        300, 300, 300, 300, 300, 300, 600,
        300, 300, 300, 300, 300, 300, 600
};

static const float song_freqs2[] = {
        G5, G5, A5, G5, C6, B5,
        G5, G5, A5, G5, D6, C6,
        G5, G5, G6, E6, C6, B5, A5,
        F6, F6, E6, C6, D6, C6
};

static const uint32_t song_durations2[] = {
        200, 100, 300, 300, 300, 600,
        200, 100, 300, 300, 300, 600,
        200, 100, 300, 300, 300, 300, 600,
        200, 100, 300, 300, 300, 600
};

static const float alarm_beep[] = {
        1000.0f, 1200.0f, 1400.0f, 1600.0f, 1800.0f
};

static const uint32_t alarm_durs[] = {
        100, 100, 100, 100, 400
};

static const float nokia_freqs[] = {
        659.25f,
        659.25f,
        659.25f,
        523.25f,
        783.99f,
        659.25f,
        523.25f,
        392.00f,
        0.0f,
        523.25f,
        0.0f,
        392.00f,
        0.0f
};

static const uint32_t nokia_durs[] = {
        150, 150, 300, 300,
        300, 150, 150, 600,
        100,
        300,
        100,
        600,
        300
};

static const float nokia_tune_freqs[] = {
        E5 * 2, D5 * 2, FS4 * 2, GS4 * 2,
        CS5 * 2, B4 * 2, D4 * 2, E4 * 2,
        B4 * 2, A4 * 2, CS4 * 2, E4 * 2,
        A4 * 2
};

static const uint32_t nokia_tune_durs[] = {
        150, 150, 300, 300,
        150, 150, 300, 300,
        150, 150, 300, 300,
        600
};

void MotorTone_PlayStartupMelody(void)
{
    MotorTone_PlayMelody(startup_freqs, startup_durs, 4, 2.0f);
}

void MotorTone_PlayPreset(uint8_t preset_id)
{
    switch (preset_id) {
        case 1:
            MotorTone_PlayMelody(song_freqs, song_durations, 14, 3.5f);
            break;
        case 2:
            MotorTone_PlayMelody(song_freqs2, song_durations2, 25, 1.5f);
            break;
        case 3:
            MotorTone_PlayMelody(nokia_freqs, nokia_durs, 13, 1.8f);
            break;
        case 4:
            MotorTone_PlayMelody(alarm_beep, alarm_durs, 5, 1.5f);
            break;
        case 5:
            MotorTone_PlayMelody(nokia_tune_freqs, nokia_tune_durs, 13, 1.8f);
            break;
        default:
            break;
    }
}
