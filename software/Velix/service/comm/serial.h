//
// Created by 18135 on 26-1-17.
//

#ifndef SPIN_MASTER_V2_SERIAL_H
#define SPIN_MASTER_V2_SERIAL_H

#include "config.h"

Velix_Status UART_DMA_Send(Velix_UartHandle *uart, const uint8_t *data, uint16_t bytes);
Velix_Status UART_DMA_Receive_Init(Velix_UartHandle *uart, uint8_t *buffer, uint16_t bytes);
Velix_UartHandle *Serial_GetCommUart(void);
void Serial_HandleIdleIRQ(Velix_UartHandle *uart);
void Serial_ParseCommand(void);
Velix_Status VOFA_SendFloats(const float *data, uint16_t count);
void VOFA_SendChannels(float ch1, float ch2, float ch3, float ch4);
void VOFA_Init(void);
extern uint8_t serial_dma_rx_buf[RX_BUF_SIZE];
extern volatile uint32_t rx_write_pos;

#endif //SPIN_MASTER_V2_SERIAL_H
