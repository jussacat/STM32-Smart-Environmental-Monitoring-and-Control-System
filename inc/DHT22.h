#ifndef DHT22_H
#define DHT22_H

#include "stm32f10x.h" 

#define DHT22_OK        0
#define DHT22_ERROR     -1

typedef struct {
    float temperature;
    float humidity;
} DHT22_Data_t;

void DHT22_Init(void);

// DHT22_OK = 0 if success, else DHT22_ERROR = -1
int8_t DHT22_Read(DHT22_Data_t *out_data);

void delay_ms(uint32_t ms);
#endif /* DHT22_H */