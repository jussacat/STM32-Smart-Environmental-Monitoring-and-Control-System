#include "task_manager.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h" //Mutex

#include "display.h"
#include "DHT22.h"
#include "RTC.h"
#include "uart.h"
#include <stdio.h>


static SemaphoreHandle_t xHardwareMutex;

extern void FloatToString(float val, char* buff, char* prefix); 


// TASK 1: RTC (1000ms)
static void vTask_RTC(void *pvParameters) {
    (void)pvParameters;
    RTC_Time_t current_rtc;
    
    while(1) {
        RTC_GetTime(&current_rtc);
        
        if (xSemaphoreTake(xHardwareMutex, portMAX_DELAY) == pdTRUE) {
            Display_PrintTime(&current_rtc);
            xSemaphoreGive(xHardwareMutex);
        }

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}


// TASK 2: SENSOR (2500ms)
static void vTask_DHT22(void *pvParameters) {
    (void)pvParameters;
    DHT22_Data_t sensor_data;
    int8_t ret;
    
    while(1) {
        ret = DHT22_Read(&sensor_data);
        
        if (xSemaphoreTake(xHardwareMutex, portMAX_DELAY) == pdTRUE) {
            
            Display_PrintTemp(ret, &sensor_data);            
            xSemaphoreGive(xHardwareMutex);
        }

        vTaskDelay(pdMS_TO_TICKS(2500));
    }
}

void OS_Run(uint8_t hour, uint8_t min, uint8_t sec, 
            uint8_t day, uint8_t date, uint8_t month, uint16_t year) {
    
    RTC_Time_t startup_rtc;

    Display_Init();

    // Time settings
    startup_rtc.hour = hour; startup_rtc.min = min; startup_rtc.sec = sec;
    startup_rtc.day = day; startup_rtc.date = date; startup_rtc.month = month; startup_rtc.year = year;
    RTC_SetTime(&startup_rtc);

    // Mutex initial
    xHardwareMutex = xSemaphoreCreateMutex();
                
    // Task create
    if (xHardwareMutex != NULL) {
        xTaskCreate(vTask_RTC,   "Task_RTC",   256, NULL, 2, NULL);
        xTaskCreate(vTask_DHT22, "Task_DHT22", 256, NULL, 1, NULL);
        vTaskStartScheduler();
    }
}