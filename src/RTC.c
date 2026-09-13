#include "RTC.h"
#include "stm32f10x.h"
#include "Oled.h"
#include "DHT22.h"
#include "string.h"
#include <stdio.h>

#define RTC_ADDR 0x68

const char *DayName[] =
{
    "",
    "SUN",
    "MON",
    "TUE",
    "WED",
    "THU",
    "FRI",
    "SAT"
};

static uint8_t DecToBCD(uint8_t dec)
{
    return ((dec / 10) << 4) | (dec % 10);
}

static uint8_t BCDToDec(uint8_t bcd)
{
    return ((bcd >> 4) * 10) + (bcd & 0x0F);
}

/*********************I2C2 PROTOCOL ***********************/
static void I2C2_Init(void){
    RCC->APB2ENR |= RCC_APB2ENR_IOPBEN | RCC_APB2ENR_AFIOEN;
    RCC->APB1ENR |= RCC_APB1ENR_I2C2EN;

    //PB10(SCL) & PB11(SDA) Alternate Function Open-Drain, 50MHz
    GPIOB->CRH &= ~(0xF << 8); //1111, reset bit PB6
    GPIOB->CRH |= (0xF << 8);

    GPIOB->CRH &= ~(0xF << 12); //1111, reset bit PB7
    GPIOB->CRH |= (0xF << 12);

    //Reset I2C2
    I2C2->CR1 |= (1 << 15U); // SWRST = 1
    I2C2->CR1 &= ~(1 << 15U); // SWRST = 0, end reset

    //Speed
    I2C2->CR2 &= ~0x3F;
    I2C2->CR2 |= 36; //FREQ[5:0] = 100100 = 36Mhz = clock APB1
    I2C2->CCR &= ~0xFFF;
    I2C2->CCR |= 180; //CCR = PLCK1 / (2 * SCL) = (36Mhz / (2 * 100kHz) = 180)
    I2C2->TRISE &= ~0x3F;
    I2C2->TRISE |= 37; //FREQ + 1

    //EN I2C2
    I2C2->CR1 |= (1 << 0U); //PE
}

static void I2C2_Start(void){
    I2C2->CR1 |= (1 << 8U); //START
    while (!(I2C2->SR1 & (1 << 0U))); //wait for SB
}

static void I2C2_SendAddress(uint8_t addr){
    I2C2->DR = addr;
    while(!(I2C2->SR1 & (1 << 1U)));
    (void)I2C2->SR1;
    (void)I2C2->SR2;
}

static void I2C2_Write(uint8_t data){
    while(!(I2C2->SR1 & (1 << 7U)));
    I2C2->DR = data;
}

uint8_t I2C2_Read_NACK(void){
    I2C2->CR1 &= ~(1 << 10U);
    while(!((I2C2->SR1) & (1 << 6U)));
    return I2C2->DR;
}

uint8_t I2C2_Read_ACK(void){
    I2C2->CR1 |= (1 << 10U);
    while(!((I2C2->SR1) & (1 << 6U)));
    return I2C2->DR;
}

static void I2C2_Stop(void){
    //while(!(I2C2->SR1 & (1 << 7U)));
    //While(!(I2C2->SR1 & (1 << 2U)));
    I2C2->CR1 |= (1 << 9U); //STOP
}
/*********************************** RTC_DS3231 **********************************/
static void RTC_WriteReg(uint8_t reg, uint8_t data)
{
    I2C2_Start();
    I2C2_SendAddress((RTC_ADDR << 1) | 0);
    I2C2_Write(reg);
    I2C2_Write(data);
    while(!(I2C2->SR1 & (1 << 2U)));
    I2C2_Stop();
}

static uint8_t RTC_ReadReg(uint8_t reg)
{
    uint8_t data;
    I2C2_Start();
    I2C2_SendAddress((RTC_ADDR << 1) | 0);
    I2C2_Write(reg);
    while(!(I2C2->SR1 & (1 << 2U)));
    I2C2_Start();
    I2C2_SendAddress((RTC_ADDR << 1) | 1);
    data = I2C2_Read_NACK();
    I2C2_Stop();
    return data;
}

void RTC_Init(void){

    I2C2_Init();

    uint8_t ctrl = RTC_ReadReg(0x0E);

    ctrl &= ~0x03; //tat A1IE / A2IE
    
    RTC_WriteReg(0x0E, ctrl);

}

void RTC_SetTime(RTC_Time_t *t)
{
    I2C2_Start();

    I2C2_SendAddress((RTC_ADDR << 1) | 0);

    I2C2_Write(0x00);

    I2C2_Write(DecToBCD(t->sec));
    I2C2_Write(DecToBCD(t->min));
    I2C2_Write(DecToBCD(t->hour));

    I2C2_Write(DecToBCD(t->day));
    I2C2_Write(DecToBCD(t->date));
    I2C2_Write(DecToBCD(t->month));

    I2C2_Write(DecToBCD(t->year - 2000));

    while(!(I2C2->SR1 & (1 << 2U)));

    I2C2_Stop();
}

void RTC_GetTime(RTC_Time_t *t)
{
    I2C2_Start();
    I2C2_SendAddress((RTC_ADDR << 1) | 0);

    I2C2_Write(0x00);
    while(!(I2C2->SR1 & (1 << 2U)));
    I2C2_Start();

    I2C2_SendAddress((RTC_ADDR << 1) | 1);

    t->sec   = BCDToDec(I2C2_Read_ACK() & 0x7F);
    t->min   = BCDToDec(I2C2_Read_ACK());
    t->hour  = BCDToDec(I2C2_Read_ACK() & 0x3F);

    t->day   = BCDToDec(I2C2_Read_ACK());
    t->date  = BCDToDec(I2C2_Read_ACK());
    t->month = BCDToDec(I2C2_Read_ACK() & 0x1F);

    t->year  = 2000 + BCDToDec(I2C2_Read_NACK());
    I2C2_Stop();
}
