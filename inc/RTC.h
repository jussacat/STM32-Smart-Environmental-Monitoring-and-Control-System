#ifndef __RTC_H
#define __RTC_H

#include "stm32f10x.h"
#include <stdint.h>
#include "DHT22.h"

typedef enum
{
    SUNDAY    = 1,
    MONDAY    = 2,
    TUESDAY   = 3,
    WEDNESDAY = 4,
    THURSDAY  = 5,
    FRIDAY    = 6,
    SATURDAY  = 7

} RTC_DayOfWeek_t;

typedef struct{
    uint8_t sec;
    uint8_t min;
    uint8_t hour;
    RTC_DayOfWeek_t day;
    uint8_t date;
    uint8_t month;
    uint16_t year;
} RTC_Time_t;

extern const char *DayName[];

void RTC_Init(void);

void RTC_SetTime(RTC_Time_t *t);

void RTC_GetTime(RTC_Time_t *t);

#endif /* __RTC_H */