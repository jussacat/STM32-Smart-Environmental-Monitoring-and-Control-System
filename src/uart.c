#include "uart.h"
#include <stdio.h> // Chứa nguyên mẫu hàm printf

void UART1_Init(uint32_t baudrate) {
    // 1. Cấp xung clock cho GPIOA và USART1 (Nằm trên bus APB2)
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_USART1EN;

    // 2. Cấu hình chân PA9 (TX)
    // Xóa cấu hình cũ (bits 4,5,6,7 của CRL tương ứng với PA9 nằm ở thanh ghi CRH)
    // Chú ý: Chân 8-15 thuộc thanh ghi CRH
    GPIOA->CRH &= ~(0xF << 4);
    // Cấu hình Output 50MHz (MODE9=11) + Alternate Function Push-Pull (CNF9=10) -> Giá trị 1011 (0xB)
    GPIOA->CRH |= (0xB << 4);

    // 3. Cấu hình chân PA10 (RX)
    GPIOA->CRH &= ~(0xF << 8);
    // Cấu hình Input (MODE10=00) + Floating Input (CNF10=01) -> Giá trị 0100 (0x4)
    GPIOA->CRH |= (0x4 << 8);

    // 4. Cấu hình Baudrate
    // Công thức tính thanh ghi BRR của STM32F1 rất đẹp: BRR = F_CPU / Baudrate
    // Giả định hệ thống đang chạy ở xung nhịp mặc định 8MHz (8,000,000)
    USART1->BRR = 8000000 / baudrate;

    // 5. Bật USART1, Bật bộ Truyền (TE) và Nhận (RE)
    USART1->CR1 |= USART_CR1_UE | USART_CR1_TE | USART_CR1_RE;
}

void UART1_SendChar(char c) {
    // Chờ cho đến khi cờ TXE (Transmit Data Register Empty) được bật lên 1
    // Tức là bộ đệm đã trống, sẵn sàng nhận byte mới
    while (!(USART1->SR & USART_SR_TXE));
    
    // Đẩy dữ liệu vào thanh ghi DR (Data Register)
    USART1->DR = (c & 0xFF);
}

/* ====================================================================
 * LIÊN KẾT PRINTF VỚI UART
 * ==================================================================== */
// Ghi đè hàm _write của thư viện GCC (newlib)
int _write(int file, char *ptr, int len) {
    (void)file; // Tránh cảnh báo unused parameter
    for (int i = 0; i < len; i++) {
        UART1_SendChar(*ptr++);
    }
    return len;
}