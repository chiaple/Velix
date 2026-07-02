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
void CalibrateCurrentOffset(SAMPLE_STRUCT *sample) {
    VELIX_ADC_START_REG_CONVERSION();

    // 等待 Rank1 完成
    while (!VELIX_ADC_REG_EOC()) {}
    sample->IuOffset = VELIX_ADC_READ_REG12();

    // 等待 Rank2 完成
    while (!VELIX_ADC_REG_EOC()) {}
    sample->IvOffset = VELIX_ADC_READ_REG12();

    //等待 Rank3 完成
    while (!VELIX_ADC_REG_EOC()) {}
    sample->IwOffset = VELIX_ADC_READ_REG12();

    //sprintf(vel,"%d,%d,%d\n",Mt.sample.IuOffset ,Mt.sample.IvOffset ,Mt.sample.IwOffset );
    HAL_Delay(100);
}

/**
 * @author
 *
 * */
void Calculate_Phase_Current(SAMPLE_STRUCT *sample, FOC_STRUCT *foc){
    sample->IuRaw = VELIX_ADC_READ_INJ12(VELIX_ADC_IU_RANK);
    sample->IvRaw = VELIX_ADC_READ_INJ12(VELIX_ADC_IV_RANK);
    sample->IwRaw = VELIX_ADC_READ_INJ12(VELIX_ADC_IW_RANK);

    //三相电压
    const float32_t u_1 = ADC_VREF * ((float32_t)(sample->IuRaw - sample->IuOffset) / ADC_RESOLUTION);
    const float32_t u_2 = ADC_VREF * ((float32_t)(sample->IvRaw - sample->IvOffset) / ADC_RESOLUTION);
    const float32_t u_3 = ADC_VREF * ((float32_t)(sample->IwRaw - sample->IwOffset) / ADC_RESOLUTION);
    //三相电流
//    p->IuReal = u_1 / (SAMPLING_RESITOR * MAGNIFICATION);
//    p->IvReal = u_2 / (SAMPLING_RESITOR * MAGNIFICATION);
//    p->IwReal = u_3 / (SAMPLING_RESITOR * MAGNIFICATION);
    foc->iu = u_1 / (SAMPLING_RESITOR * MAGNIFICATION);
    foc->iv = u_2 / (SAMPLING_RESITOR * MAGNIFICATION);
    foc->iw = u_3 / (SAMPLING_RESITOR * MAGNIFICATION);
}
