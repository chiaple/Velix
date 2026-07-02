#ifndef SPIN_MASTER_V2_TONE_PRESETS_H
#define SPIN_MASTER_V2_TONE_PRESETS_H

#include <stdint.h>

void MotorTone_PlayStartupMelody(void);
void MotorTone_PlayPreset(uint8_t preset_id);

#endif // SPIN_MASTER_V2_TONE_PRESETS_H
