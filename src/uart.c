#include "uart.h"

void UART1_Init(uint32_t baudrate) {
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_USART1EN;

    // PA9 (TX)
    GPIOA->CRH &= ~(0xF << 4);
    //Output 50MHz (MODE9=11) + Alternate Function Push-Pull (CNF9=10) ->1011 (0xB)
    GPIOA->CRH |= (0xB << 4);

    //PA10 (RX)
    GPIOA->CRH &= ~(0xF << 8);
    //Input (MODE10=00) + Floating Input (CNF10=01) ->0100 (0x4)
    GPIOA->CRH |= (0x4 << 8);

    //Baudrate
    // BRR = F_CPU / Baudrate
    // 8MHz (8,000,000)
    USART1->BRR = 8000000 / baudrate;

    USART1->CR1 |= USART_CR1_UE | USART_CR1_TE | USART_CR1_RE;
}

void UART1_SendChar(char c) {
    while (!(USART1->SR & USART_SR_TXE));
    USART1->DR = (c & 0xFF);
}

char UART1_ReceiveChar(void) {
    while (!(USART1->SR & (1 << 5))); 
    return (char)(USART1->DR & 0xFF);
}

void UART1_Send2Digit(uint8_t num) {
    UART1_SendChar((num / 10) + '0'); // Tens
    UART1_SendChar((num % 10) + '0'); // Ones
}

void UART1_SendInt(int32_t num) {
    if (num < 0) {
        UART1_SendChar('-');
        num = -num;
    }
    if (num == 0) {
        UART1_SendChar('0');
        return;
    }
    
    char buf[10];
    int i = 0;
    while (num > 0) {
        buf[i++] = (num % 10) + '0';
        num /= 10;
    }
    while (i > 0) {
        UART1_SendChar(buf[--i]);
    }
}

void UART1_SendFloat1Dec(float num) {
    if (num < 0) {
        UART1_SendChar('-');
        num = -num;
    }
    
    int int_part = (int)num;
    int frac_part = (int)(num * 10.0f) % 10; 
    
    UART1_SendInt(int_part);
    UART1_SendChar('.');
    UART1_SendChar(frac_part + '0');
}

void UART1_SendString(const char *str) {
    while (*str) {
        UART1_SendChar(*str++);
    }
}