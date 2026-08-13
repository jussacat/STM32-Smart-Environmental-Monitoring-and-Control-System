#ifndef UART_H
#define UART_H

#include "stm32f10x.h"

// Khởi tạo UART1 với tốc độ truyền (Baudrate) mong muốn
void UART1_Init(uint32_t baudrate);

// Gửi một ký tự
void UART1_SendChar(char c);

#endif /* UART_H */