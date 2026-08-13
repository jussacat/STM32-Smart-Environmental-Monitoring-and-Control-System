#ifndef OLED_H
#define OLED_H

#include "stm32f10x.h"

// Khởi tạo màn hình OLED
void OLED_Init(void);

// Xóa toàn bộ màn hình
void OLED_Clear(void);

// Đặt con trỏ chuột. x: Cột (0-127), y: Hàng (0-7)
void OLED_SetCursor(uint8_t x, uint8_t y);

// In một chuỗi ký tự ra màn hình
void OLED_PrintString(char* str);

void FloatToString(float value, char* buf, char* prefix);

#endif /* OLED_H */