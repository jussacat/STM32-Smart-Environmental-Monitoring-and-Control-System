#ifndef OLED_H
#define OLED_H

#include "stm32f10x.h"

void OLED_Init(void);

void OLED_Clear(void);

void OLED_SetCursor(uint8_t x, uint8_t y);

void OLED_PrintString(char* str);

extern void FloatToString(float value, char* buf, char* prefix);

#endif /* OLED_H */