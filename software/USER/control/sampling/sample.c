//
// Created by e on 26-1-20.
//

#include <stdio.h>
#include "sample.h"
#include "adc.h"
#include "global.h"
#include "usart.h"
#include "config.h"
/**
 *
 *  电流校准
 *
 * */
char vel[20];
void CalibrateCurrentOffset(SAMPLE_STRUCT *sample) {
    LL_ADC_REG_StartConversion(ADC1);

    // 等待 Rank1 完成
    while (!LL_ADC_IsActiveFlag_EOC(ADC1)) {}
    sample->IuOffset = LL_ADC_REG_ReadConversionData12(ADC1);

    // 等待 Rank2 完成
    while (!LL_ADC_IsActiveFlag_EOC(ADC1)) {}
    sample->IvOffset = LL_ADC_REG_ReadConversionData12(ADC1);

    //等待 Rank3 完成
    while (!LL_ADC_IsActiveFlag_EOC(ADC1)) {}
    sample->IwOffset = LL_ADC_REG_ReadConversionData12(ADC1);

    //sprintf(vel,"%d,%d,%d\n",Mt.sample.IuOffset ,Mt.sample.IvOffset ,Mt.sample.IwOffset );
    //USART_SendString(USART3,vel);
    HAL_Delay(100);
}

/**
 * @author
 *
 * */
void Calculate_Phase_Current(SAMPLE_STRUCT *sample, FOC_STRUCT *foc){
    //p->IuReal =ADC1->JDR1;
    sample->IuRaw = LL_ADC_INJ_ReadConversionData12(ADC1, LL_ADC_INJ_RANK_1);
    sample->IvRaw = LL_ADC_INJ_ReadConversionData12(ADC1, LL_ADC_INJ_RANK_2);
    sample->IwRaw = LL_ADC_INJ_ReadConversionData12(ADC1, LL_ADC_INJ_RANK_3);

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
