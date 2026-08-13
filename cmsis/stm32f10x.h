#ifndef MY_STM32_REGS_H 
#define MY_STM32_REGS_H

#include <stdint.h> 

//1. Địa chỉ gốc của các bus giao tiếp
#define PERIPH_BASE 0x40000000U 
#define APB1PERIPH_BASE PERIPH_BASE 
#define APB2PERIPH_BASE (PERIPH_BASE + 0x10000U) 
#define AHBPERIPH_BASE (PERIPH_BASE + 0x20000U)

// 2. Địa chỉ gốc của các ngoại vi cụ thể 
#define RCC_BASE        (AHBPERIPH_BASE + 0x1000U) 
#define GPIOA_BASE      (APB2PERIPH_BASE + 0x0800U) 
#define GPIOB_BASE      (APB2PERIPH_BASE + 0x0C00U)
#define GPIOC_BASE      (APB2PERIPH_BASE + 0x1000U) 
#define ADC1_BASE       (APB2PERIPH_BASE + 0x2400U)
#define TIM1_BASE       (APB2PERIPH_BASE + 0x2C00U)     
#define USART1_BASE     (APB2PERIPH_BASE + 0x3800U)
#define TIM2_BASE       (APB1PERIPH_BASE)     
#define I2C1_BASE       (APB1PERIPH_BASE + 0x5400U)
#define I2C2_BASE       (APB1PERIPH_BASE + 0x5800U)

typedef struct {  
    volatile uint32_t CR; // Offset: 0x00 
    volatile uint32_t CFGR; // Offset: 0x04 
    volatile uint32_t CIR; // Offset: 0x08 
    volatile uint32_t APB2RSTR; // Offset: 0x0C 
    volatile uint32_t APB1RSTR; // Offset: 0x10 
    volatile uint32_t AHBENR; // Offset: 0x14 
    volatile uint32_t APB2ENR; // Offset: 0x18 
    volatile uint32_t APB1ENR; // Offset: 0x1C
} RCC_typedef;

typedef struct { 
    volatile uint32_t CRL; // Offset: 0x00 
    volatile uint32_t CRH; // Offset: 0x04 
    volatile uint32_t IDR; // Offset: 0x08 
    volatile uint32_t ODR; // Offset: 0x0C 
    volatile uint32_t BSRR; // Offset: 0x10 
    volatile uint32_t BRR; // Offset: 0x14 
    volatile uint32_t LCKR; // Offset: 0x18 
} GPIO_typedef;

typedef struct {
    volatile uint32_t ADC_SR; //Offset: 0x00
    volatile uint32_t ADC_CR1; // Offset: 0x04
    volatile uint32_t ADC_CR2; // Offset: 0x08 
    volatile uint32_t ADC_SMPR1; // Offset: 0x0C 
    volatile uint32_t ADC_SMPR2; // Offset: 0x10 
    volatile uint32_t ADC_JOFR1; // Offset: 0x14 
    volatile uint32_t ADC_JOFR2; // Offset: 0x18 
    volatile uint32_t ADC_JOFR3; //Offset: 0x1C
    volatile uint32_t ADC_JOFR4; // Offset: 0x20
    volatile uint32_t ADC_HTR; // Offset: 0x24
    volatile uint32_t ADC_LTR; //Offset: 0x28
    volatile uint32_t ADC_SQR1; // Offset: 0x2C
    volatile uint32_t ADC_SQR2; // Offset: 0x30
    volatile uint32_t ADC_SQR3; //Offset: 0x34
    volatile uint32_t ADC_JSQR; // Offset: 0x38 
    volatile uint32_t ADC_JDR1; // Offset: 0x3C 
    volatile uint32_t ADC_JDR2; // Offset: 0x40 
    volatile uint32_t ADC_JDR3; // Offset: 0x44 
    volatile uint32_t ADC_JDR4; // Offset: 0x48 
    volatile uint32_t ADC_DR; //Offset: 0x4C
} ADC_typedef;

typedef struct {
    volatile uint32_t CR1; //Offset: 0x00
    volatile uint32_t CR2; // Offset: 0x04
    volatile uint32_t SMCR; // Offset: 0x08 
    volatile uint32_t DIER; // Offset: 0x0C 
    volatile uint32_t SR; // Offset: 0x10 
    volatile uint32_t EGR; // Offset: 0x14 
    volatile uint32_t CCMR1; // Offset: 0x18 
    volatile uint32_t CCMR2; //Offset: 0x1C
    volatile uint32_t CCER; // Offset: 0x20
    volatile uint32_t CNT; // Offset: 0x24
    volatile uint32_t PSC; //Offset: 0x28
    volatile uint32_t ARR; // Offset: 0x2C
    volatile uint32_t RCR; // Offset: 0x30
    volatile uint32_t CCR1; //Offset: 0x34
    volatile uint32_t CCR2;// Offset: 0x38 
    volatile uint32_t CCR3; // Offset: 0x3C 
    volatile uint32_t CCR4; // Offset: 0x40 
    volatile uint32_t BDTR; // Offset: 0x44 
    volatile uint32_t DCR; // Offset: 0x48 
    volatile uint32_t DMAR; //Offset: 0x4C
} TIMER_typedef;

typedef struct { 
    volatile uint32_t SR; // Offset: 0x00 
    volatile uint32_t DR; // Offset: 0x04 
    volatile uint32_t BRR; // Offset: 0x08 
    volatile uint32_t CR1; // Offset: 0x0C 
    volatile uint32_t CR2; // Offset: 0x10 
    volatile uint32_t CR3; // Offset: 0x14 
    volatile uint32_t GTPR; // Offset: 0x18 
} USART_typedef;

typedef struct {
    volatile uint32_t CR1; //Offset: 0x00
    volatile uint32_t CR2; // Offset: 0x04
    volatile uint32_t OAR1; // Offset: 0x08 
    volatile uint32_t OAR2; // Offset: 0x0C 
    volatile uint32_t DR; // Offset: 0x10 
    volatile uint32_t SR1; // Offset: 0x14 
    volatile uint32_t SR2; // Offset: 0x18 
    volatile uint32_t CCR; //Offset: 0x1C
    volatile uint32_t TRISE; // Offset: 0x20
} I2C_typedef;

//Ép kiểu các Base thành con trỏ struct
#define RCC ((RCC_typedef *) RCC_BASE)
#define GPIOC  ((GPIO_typedef *) GPIOC_BASE)
#define GPIOB ((GPIO_typedef *) GPIOB_BASE)
#define GPIOA ((GPIO_typedef *) GPIOA_BASE)
#define ADC1 ((ADC_typedef *) ADC1_BASE)
#define TIM1 ((TIMER_typedef *) TIM1_BASE)
#define TIM2 ((TIMER_typedef *) TIM2_BASE)
#define USART1 ((USART_typedef *) USART1_BASE)
#define I2C1 ((I2C_typedef *) I2C1_BASE)
#define I2C2 ((I2C_typedef *) I2C2_BASE)

/******************  Bit definition for RCC_APB1ENR register  *****************/
#define RCC_APB1ENR_TIM2EN_Pos                  (0U) //Vi tri bit => TIM2EN = bit 0
#define RCC_APB1ENR_TIM2EN_Msk                  (0x1UL << RCC_APB1ENR_TIM2EN_Pos) //MSK bit => 0x00000001
#define RCC_APB1ENR_TIM2EN                      RCC_APB1ENR_TIM2EN_Msk /*Timer2 clock enabled*/
#define RCC_APB1ENR_I2C1EN_Pos                  (21U) 
#define RCC_APB1ENR_I2C1EN_Msk                  (0x1UL << RCC_APB1ENR_I2C1EN_Pos) //MSK bit => 0x00000001
#define RCC_APB1ENR_I2C1EN                      RCC_APB1ENR_I2C1EN_Msk 
#define RCC_APB1ENR_I2C2EN_Pos                  (22U) 
#define RCC_APB1ENR_I2C2EN_Msk                  (0x1UL << RCC_APB1ENR_I2C2EN_Pos) //MSK bit => 0x00000001
#define RCC_APB1ENR_I2C2EN                      RCC_APB1ENR_I2C2EN_Msk 


/******************  Bit definition for RCC_APB2ENR register  *****************/
#define RCC_APB2ENR_AFIOEN_Pos                  (0U) 
#define RCC_APB2ENR_AFIOEN_Msk                  (0x1UL << RCC_APB2ENR_AFIOEN_Pos) //MSK bit => 0x00000001
#define RCC_APB2ENR_AFIOEN                      RCC_APB2ENR_AFIOEN_Msk 
#define RCC_APB2ENR_IOPAEN_Pos                  (2U) 
#define RCC_APB2ENR_IOPAEN_Msk                  (0x1UL << RCC_APB2ENR_IOPAEN_Pos) //MSK bit => 0x00000001
#define RCC_APB2ENR_IOPAEN                      RCC_APB2ENR_IOPAEN_Msk 
#define RCC_APB2ENR_IOPBEN_Pos                  (3U) 
#define RCC_APB2ENR_IOPBEN_Msk                  (0x1UL << RCC_APB2ENR_IOPBEN_Pos) //MSK bit => 0x00000001
#define RCC_APB2ENR_IOPBEN                      RCC_APB2ENR_IOPBEN_Msk 
#define RCC_APB2ENR_USART1EN_Pos                (14U) 
#define RCC_APB2ENR_USART1EN_Msk                (0x1UL << RCC_APB2ENR_USART1EN_Pos) //MSK bit => 0x00000001
#define RCC_APB2ENR_USART1EN                    RCC_APB2ENR_USART1EN_Msk 


/*******************  Bit definition for TIM_CR1 register  *******************/
#define TIM_CR1_CEN_Pos                      (0U)
#define TIM_CR1_CEN_Msk                      (0x1UL << TIM_CR1_CEN_Pos)
#define TIM_CR1_CEN                          TIM_CR1_CEN_Msk


/*******************  Bit definition for USART_CR1 register  *******************/
#define USART_CR1_UE_Pos                      (13U)
#define USART_CR1_UE_Msk                      (0x1UL << USART_CR1_UE_Pos)
#define USART_CR1_UE                          USART_CR1_UE_Msk 
#define USART_CR1_TE_Pos                      (3U)
#define USART_CR1_TE_Msk                      (0x1UL << USART_CR1_TE_Pos)
#define USART_CR1_TE                          USART_CR1_TE_Msk 
#define USART_CR1_RE_Pos                      (2U)
#define USART_CR1_RE_Msk                      (0x1UL << USART_CR1_RE_Pos)
#define USART_CR1_RE                          USART_CR1_RE_Msk 


/*******************  Bit definition for USART_SR register  *******************/
#define USART_SR_TXE_Pos                      (7U)
#define USART_SR_TXE_Msk                      (0x1UL << USART_SR_TXE_Pos)
#define USART_SR_TXE                          USART_SR_TXE_Msk 


/*******************  Bit definition for GPIOB register  *******************/

#endif //MY_STM32_REGS_H