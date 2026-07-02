//
// Created by 18135 on 26-1-17.
//

#include "MT6701.h"
#include "spi.h"
#include "FOC.h"


uint16_t MT6701_ReadRaw(void) {
    uint16_t raw = 0;
    uint16_t dummy = 0x0000;

    // 1. 片选拉低
    MT6701_CS_LOW;

    // 2. 调用 HAL 全双工传输函数
    // 参数：句柄，发送缓冲区指针，接收缓冲区指针，数据长度(按DataSize计)，超时时间(ms)
    // 注意：HAL 内部会自动处理 TXE 和 BSY 标志位，不需要手动 while 判断
    if (HAL_SPI_TransmitReceive(&hspi1, (uint8_t *)&dummy, (uint8_t *)&raw, 1, 10) != HAL_OK) {
        // 可以在这里做错误处理
    }

    // 3. 片选拉高
    MT6701_CS_HIGH;

    // 4. 返回处理后的数据
    return raw >> 2;
}
