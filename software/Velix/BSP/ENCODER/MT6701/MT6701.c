//
// Created by 18135 on 26-1-17.
//

#include "MT6701.h"
#include "FOC.h"


uint16_t MT6701_ReadRaw(void) {
    uint16_t raw = 0;
    uint16_t dummy = 0x0000;

    // 1. 片选拉低
    VELIX_ENCODER_CS_LOW();

    // 2. 读取编码器原始数据
    if (Velix_EncoderTransfer16(dummy, &raw, 10) != VELIX_OK) {
        // 可以在这里做错误处理
    }

    // 3. 片选拉高
    VELIX_ENCODER_CS_HIGH();

    // 4. 返回处理后的数据
    return raw >> 2;
}
