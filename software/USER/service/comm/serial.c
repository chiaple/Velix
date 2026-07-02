//
// Created by 18135 on 26-1-17.
//

#include "stdio.h"
#include "serial.h"
#include "global.h"
#include "serial_commands.h"

#if (VELIX_COMM_UART_PORT != 1) && (VELIX_COMM_UART_PORT != 3)
#error "VELIX_COMM_UART_PORT must be 1 or 3"
#endif

uint8_t serial_dma_rx_buf[RX_BUF_SIZE]; // DMA 循环接收缓冲区
volatile uint32_t rx_write_pos = 0;   // DMA 当前写到的位置（head）
volatile uint32_t rx_read_pos  = 0;   // 应用已处理到的位置（tail）


// 4 字节对齐
__attribute__((aligned(4)))
static uint8_t vofa_tx_buf[VOFA_FRAME_BYTES(VOFA_MAX_CH_COUNT)];

Velix_UartHandle *Serial_GetCommUart(void)
{
    return Velix_UartCommHandle();
}

void Serial_HandleIdleIRQ(Velix_UartHandle *huart)
{
    if ((huart == NULL) || (huart != Serial_GetCommUart()))
    {
        return;
    }

    if (Velix_UartIsIdle(huart))
    {
        Velix_UartClearIdle(huart);
        rx_write_pos = RX_BUF_SIZE - Velix_UartDmaRemaining(huart);
    }
}

Velix_Status UART_DMA_Receive_Init(Velix_UartHandle *huart, uint8_t *buffer, uint16_t bytes)
{
    if (huart == NULL || buffer == NULL || bytes == 0U)
    {
        return VELIX_ERROR;
    }

    if (Velix_UartStartDmaRx(huart, buffer, bytes) != VELIX_OK)
    {
        return VELIX_ERROR;
    }

    Velix_UartEnableIdleIrq(huart);
    Velix_UartEnable(huart);
    return VELIX_OK;
}
// 指令解析函数（主循环或 FOC 任务里反复调用）
void Serial_ParseCommand(void)
{
    // 只要有未处理的数据就一直解析（支持多条指令连续到来）
    while (rx_read_pos != rx_write_pos)
    {
        // 计算当前可读字节数（考虑环形缓冲区有两种情况）
        uint32_t len_available;
        if (rx_write_pos >= rx_read_pos)
        {
            len_available = rx_write_pos - rx_read_pos;
        }
        else
        {
            len_available = RX_BUF_SIZE - rx_read_pos + rx_write_pos;
        }

        // 如果数据太少，不够一条指令，退出等下次
        if (len_available < 4) break;  // 假设最短指令也要几个字节

        // 从 rx_read_pos 开始查找完整指令
        uint32_t start = rx_read_pos;
        uint32_t cmd_len = 0;
        uint8_t found = 0;

        for (uint32_t i = 0; i < len_available; i++)
        {
            uint32_t pos = (start + i) % RX_BUF_SIZE;

            if (serial_dma_rx_buf[pos] == '\r')
            {
                uint32_t next_pos = (pos + 1) % RX_BUF_SIZE;
                if (serial_dma_rx_buf[next_pos] == '\n')
                {
                    // 找到 \r\n 结束符
                    cmd_len = i + 2;  // 包含 \r\n
                    found = 1;
                    break;
                }
            }
        }

        if (!found) break;  // 还没收到完整指令，等下次 IDLE

        // 现在有一条完整指令：从 start 到 (start + cmd_len - 1)
        // 提取并处理
        process_single_command(start, cmd_len);

        // 推进读指针（环形）
        rx_read_pos = (rx_read_pos + cmd_len) % RX_BUF_SIZE;
    }

    // 可选：翻转 LED 表示有新指令被处理（放在 while 外面或里面都行）
    // LL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
}



// =============================================

// VOFA+ JustFloat 固定帧尾（小端 +Inf）
static const uint8_t justfloat_tail[4] = {0x00, 0x00, 0x80, 0x7F};
// =============================================
// 初始化
void VOFA_Init(void)
{
    // 只需设置一次帧尾
    memcpy(&vofa_tx_buf[VOFA_FRAME_BYTES(VOFA_MAX_CH_COUNT) - 4U], justfloat_tail, 4U);

    // 可选：清零数据区，防止启动时发垃圾值
    memset(vofa_tx_buf, 0, sizeof(vofa_tx_buf) - 4U);
}

// =============================================
//===================DMA发送
Velix_Status UART_DMA_Send(Velix_UartHandle *huart, const uint8_t *data, uint16_t bytes)
{
    if (huart == NULL || data == NULL || bytes == 0U)
    {
        return VELIX_ERROR;
    }

    // 空闲时启动新的 DMA 发送，避免打断上次发送。
    if (!Velix_UartIsReady(huart))
    {
        return VELIX_BUSY;
    }

    return Velix_UartStartDmaTx(huart, data, bytes);
}

// ================vofa发送=============================
Velix_Status VOFA_SendFloats(const float *data, uint16_t count)
{
    if (data == NULL || count == 0U || count > VOFA_MAX_CH_COUNT)
    {
        return VELIX_ERROR;
    }

    for (uint16_t i = 0; i < count; i++)
    {
        memcpy(&vofa_tx_buf[i * 4U], &data[i], sizeof(float));
    }

    memcpy(&vofa_tx_buf[count * 4U], justfloat_tail, 4U);
    return UART_DMA_Send(Serial_GetCommUart(), vofa_tx_buf, VOFA_FRAME_BYTES(count));
}

void VOFA_SendChannels(float ch1, float ch2, float ch3, float ch4)
{
    const float frame[4] = {ch1, ch2, ch3, ch4};
    (void)VOFA_SendFloats(frame, 4U);
}
