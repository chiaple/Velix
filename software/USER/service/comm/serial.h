//
// Created by 18135 on 26-1-17.
//

#ifndef SPIN_MASTER_V2_SERIAL_H
#define SPIN_MASTER_V2_SERIAL_H

#include "config.h"

HAL_StatusTypeDef UART_DMA_Send(UART_HandleTypeDef *huart, const uint8_t *data, uint16_t bytes);
HAL_StatusTypeDef UART_DMA_Receive_Init(UART_HandleTypeDef *huart, uint8_t *buffer, uint16_t bytes);
UART_HandleTypeDef *Serial_GetCommUart(void);
void Serial_HandleIdleIRQ(UART_HandleTypeDef *huart);
void Serial_ParseCommand(void);
HAL_StatusTypeDef VOFA_SendFloats(const float *data, uint16_t count);
void VOFA_SendChannels(float ch1, float ch2, float ch3, float ch4);
void VOFA_Init(void);
extern uint8_t serial_dma_rx_buf[RX_BUF_SIZE];
extern volatile uint32_t rx_write_pos;

#endif //SPIN_MASTER_V2_SERIAL_H
