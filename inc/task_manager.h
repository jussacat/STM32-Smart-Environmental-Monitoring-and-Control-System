#ifndef _TASK_MANAGER_H_
#define _TASK_MANAGER_H_

#include <stdint.h>

void OS_Run(uint8_t hour, uint8_t min, uint8_t sec, 
            uint8_t day, uint8_t date, uint8_t month, uint16_t year);

#endif /* _TASK_MANAGER_H_ */