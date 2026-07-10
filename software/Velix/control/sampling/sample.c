//
// Created by e on 26-1-20.
//

#include <stdio.h>
#include "sample.h"
#include "global.h"
#include "config.h"
/**
 *
 *  电流校准
 *
 * */
char vel[20];
void CalibrateCurrentOffset(SAMPLE_STRUCT *sample)
{
    if (sample == NULL) {
        return;
    }

    enum {
        CALIB_SAMPLES = 64,
        CALIB_TIMEOUT_MS = 20
    };

    uint32_t iu_sum = 0;
    uint32_t iv_sum = 0;
    uint32_t valid_samples = 0;

    VELIX_PWM_SET_ADC_TRIGGER(4000);

    if (VELIX_ADC_START_INJ_CONVERSION() != VELIX_OK) {
        return;
    }

    for (uint32_t i = 0; i < CALIB_SAMPLES; i++) {
        if (!VELIX_ADC_INJ_EOC(CALIB_TIMEOUT_MS)) {
            break;
        }

        iu_sum += VELIX_ADC_READ_INJ12(VELIX_ADC_IU_RANK);
        iv_sum += VELIX_ADC_READ_INJ12(VELIX_ADC_IV_RANK);
        valid_samples++;
    }

    (void)VELIX_ADC_STOP_INJ_CONVERSION();

    if (valid_samples == 0U) {
        return;
    }

    sample->IuOffset = (uint16_t)(iu_sum / valid_samples);
    sample->IvOffset = (uint16_t)(iv_sum / valid_samples);

    Velix_DelayMs(100);
}

void Sample_UpdateBusVoltage(SAMPLE_STRUCT *sample)
{
    if (sample == NULL) {
        return;
    }

    sample->BusRaw = sample->AdcBuff[0];
    sample->BusReal = ADC_VREF * ((float32_t)sample->BusRaw / ADC_RESOLUTION) * 22.27f;
}

/**
 * @author
 *
 * */
void Calculate_Phase_Current(SAMPLE_STRUCT *sample, FOC_STRUCT *foc){
    sample->IuRaw = VELIX_ADC_READ_INJ12(VELIX_ADC_IU_RANK);
    sample->IvRaw = VELIX_ADC_READ_INJ12(VELIX_ADC_IV_RANK);

    //三相电压
    const float32_t u_1 = ADC_VREF * ((float32_t)(sample->IuRaw - sample->IuOffset) / ADC_RESOLUTION);
    const float32_t u_2 = ADC_VREF * ((float32_t)(sample->IvRaw - sample->IvOffset) / ADC_RESOLUTION);
    //三相电流
//    p->IuReal = u_1 / (SAMPLING_RESITOR * MAGNIFICATION);
//    p->IvReal = u_2 / (SAMPLING_RESITOR * MAGNIFICATION);
//    p->IwReal = u_3 / (SAMPLING_RESITOR * MAGNIFICATION);
    foc->iu = u_1 / (SAMPLING_RESITOR * MAGNIFICATION);
    foc->iv = u_2 / (SAMPLING_RESITOR * MAGNIFICATION);
    foc->iw = -(foc->iu + foc->iv);
}
