#include "DHT22.h"
#include "stm32f10x.h"
#include "uart.h"
#include <stdio.h>
#include "Oled.h"
#include "RTC.h"
#include "task_manager.h"

int main(void){

    OS_Run(19, 59, 0, 4, 13, 8, 2026);
    
    while(1){
    }
    return 0;
}