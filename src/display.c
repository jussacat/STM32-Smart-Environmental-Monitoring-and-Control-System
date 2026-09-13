#include "display.h"
#include "stm32f10x.h"
#include "Oled.h"
#include "RTC.h"
#include "uart.h"
#include "DHT22.h"
#include <stdio.h>

static RTC_Time_t old_rtc = {0};
static float old_temp = -100.0f;
static float old_humi = -100.0f;
static int8_t old_status = -99;

void Display_Init(void) {

    UART1_Init(9600);

    UART1_SendString("\r\n[TEST] UART HELLO!\r\n");
    DHT22_Init();
    delay_ms(2000);
    OLED_Init();

    //Welcome Screen
    OLED_Clear();
    OLED_SetCursor(10, 0);
    OLED_PrintString("ENVI SYSTEM");
    delay_ms(2000);

    //RTC initialize
    RTC_Init();
    OLED_Clear();
    OLED_SetCursor(10, 0);
    OLED_PrintString("INIT RTC...");
    delay_ms(2000);

    
    OLED_Clear();
    UART1_SendString("\r\n--- BARE-METAL ENVI SYSTEM ---\r\n");
}

void Display_PrintTime(RTC_Time_t *rtc) {
    char buffer[64];

    // --- UPDATE SECONDS --- 
    if (rtc->sec != old_rtc.sec) {
        sprintf(buffer, "%02d:%02d:%02d", rtc->hour, rtc->min, rtc->sec);
        OLED_SetCursor(0, 3); 
        OLED_PrintString(buffer);

        // Terminal UART
        UART1_SendString(buffer);
        UART1_SendString("\r\n");

        old_rtc.hour = rtc->hour;
        old_rtc.min  = rtc->min;
        old_rtc.sec  = rtc->sec;
    }
    // --- UPDATE DATE--- 
    if (rtc->date != old_rtc.date || rtc->month != old_rtc.month || rtc->year != old_rtc.year) {
        
        sprintf(buffer, "%s, %02d/%02d/%04d", DayName[rtc->day], rtc->date, rtc->month, rtc->year);
        
        OLED_SetCursor(0, 0);
        OLED_PrintString(buffer);
        
        old_rtc.day   = rtc->day;
        old_rtc.date  = rtc->date;
        old_rtc.month = rtc->month;
        old_rtc.year  = rtc->year;
    }
}

void Display_PrintTemp(int8_t status, DHT22_Data_t *data) {
    char buffer[16];

    if (status == 0) {
        if (data->temperature != old_temp || data->humidity != old_humi || old_status != 0) {
            FloatToString(data->temperature, buffer, "TEMP: ");
            OLED_SetCursor(0, 5);
            OLED_PrintString(buffer);
            OLED_PrintString("   ");
        
            FloatToString(data->humidity, buffer, "HUMID: ");
            OLED_SetCursor(0, 7);
            OLED_PrintString(buffer);
            OLED_PrintString("   ");

            //UART
            char temp_str[16], humi_str[16];
            FloatToString(data->temperature, temp_str, "Temp: ");
            FloatToString(data->humidity, humi_str, "Humid: ");

            UART1_SendString("[DHT22] ");
            UART1_SendString(temp_str);
            UART1_SendString("C, ");
            UART1_SendString(humi_str);
            UART1_SendString("%\r\n");

            old_temp = data->temperature;
            old_humi = data->humidity;
        }
    }
    else if (status == -1) {
        if (old_status != -1) {
            OLED_SetCursor(0, 5);
            OLED_PrintString("DHT ERROR!!     ");
            OLED_SetCursor(0, 7);
            OLED_PrintString("                ");

            UART1_SendString("[DHT22] ERROR: Sensor Read Failed!\r\n");
        }
    }
    old_status = status; 
}