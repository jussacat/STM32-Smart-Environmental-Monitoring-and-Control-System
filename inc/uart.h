#ifndef UART_H
#define UART_H

#include "stm32f10x.h"

void UART1_Init(uint32_t baudrate);

void UART1_SendChar(char c);

char UART1_ReceiveChar(void);

void UART1_SendString(const char *str);
#endif /* UART_H */