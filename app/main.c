#include "DHT22.h"
#include "stm32f10x.h"
//#include "uart.h"
#include <stdio.h>
#include "Oled.h"
#include "RTC.h"

int main(void){

    RTC_PrintTime(19,30,0,4,13,8,2026);
    //DHT22_Init();
    //delay_ms(2000);
    //UART1_Init(115200);
    //OLED_Init();

    // OLED_SetCursor(10, 0);
    // OLED_PrintString("ENVI SYSTEM");

    // DHT22_Data_t sensor_data;
    // char displayBuff[16];
    // int8_t ret;
    // delay_ms(2000);
    // while(1) {
    //     ret = DHT22_Read(&sensor_data);
    //     if (ret == DHT22_OK) {
    //         OLED_Clear();
    //         FloatToString(sensor_data.temperature, displayBuff, "TEMP: ");
    //         OLED_SetCursor(0,2);
    //         OLED_PrintString(displayBuff);

    //         FloatToString(sensor_data.humidity, displayBuff, "HUMID: ");
    //         OLED_SetCursor(0,3);
    //         OLED_PrintString(displayBuff);
    //     }
    //     else {
    //         OLED_SetCursor(0,2);
    //         if(ret == -1)
    //             OLED_PrintString("DHT ERROR!!");
    //     }
    // delay_ms(2500);
    // }

    return 0;
}