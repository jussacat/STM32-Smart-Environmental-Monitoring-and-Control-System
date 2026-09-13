#ifndef _DISPLAY_H_
#define _DISPLAY_H_

#include <stdint.h>
#include "RTC.h"      
#include "Oled.h"     
#include "uart.h"     

// Welcome
void Display_Init(void);

// Data update
void Display_PrintTime(RTC_Time_t *rtc);
void Display_PrintTemp(int8_t status, DHT22_Data_t *data);

#endif /*_DISPLAY_H*/