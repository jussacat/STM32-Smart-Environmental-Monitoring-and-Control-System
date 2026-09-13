#include "DHT22.h"
#include "Oled.h"

#define DHT22_PORT  GPIOA
#define DHT22_PIN   5

/*===============DELAYUS================*/

static void TIM2_Init_us(void) {
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
    
    // Prescaler = Clock / Freq - 1 = 8000000 / 1000000 - 1 = 7
    TIM2->PSC = 8 - 1; 
    TIM2->ARR = 0xFFFF; // 65535
    
    TIM2->CR1 |= TIM_CR1_CEN; 
}

static void delay_us(uint16_t us){
    TIM2->CNT = 0; //reset 
    while (TIM2->CNT < us); 
}
/*===============DELAY MS================*/
void delay_ms(uint32_t ms)
{
    while(ms--)
    {
        delay_us(1000);
    }
}
/*=============================*/
//Output Push-Pull  
static void Set_pin_output(void){
    DHT22_PORT->CRL &= ~(0xF << 20); //0xF = mask 1111, clear bit PA5
    DHT22_PORT->CRL |=  (0x6 << 20); //0010, MODE5[1:0] = 10 (2Mhz), CNF5[1:0] = 00 (push-pull)
}

static void Set_pin_input(void){
    DHT22_PORT->CRL &= ~(0xF << 20);
    DHT22_PORT->CRL |=  (0x8 << 20); //1000; CNF5 = 10 (in pullup/down); MODE5 = 00
    DHT22_PORT->ODR |=  (1 << DHT22_PIN); //ODR pullup resistor
}

/*=========================================== USER ==============================================*/

void DHT22_Init(void){
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;

    TIM2_Init_us();

    //Output High
    Set_pin_output();
    DHT22_PORT->BSRR = (1 << DHT22_PIN);
}

int8_t DHT22_Read(DHT22_Data_t *out_data){
    uint8_t data[5] = {0,0,0,0,0};
    uint32_t timeout;

    //START SIGNAL
    Set_pin_output();
    DHT22_PORT->BSRR = (1 << (DHT22_PIN + 16)); //LOW
    delay_us(2000);

    DHT22_PORT->BSRR = (1 << DHT22_PIN); //HIGH
    delay_us(30);

    Set_pin_input(); 

    //WAIT FOR RESPONSE
    timeout = 0;
    while(DHT22_PORT->IDR & (1 << DHT22_PIN)){ // wait LOW
         if(++timeout > 1000) return DHT22_ERROR;
         delay_us(1);
    }
    

    timeout = 0;
    while(!(DHT22_PORT->IDR & (1 << DHT22_PIN))){ //wait HIGH
        if(++timeout > 1000) return DHT22_ERROR;
        delay_us(1);
    }

    timeout = 0;
    while(DHT22_PORT->IDR & (1 << DHT22_PIN)){ //LOW again
        if(++timeout > 1000) return DHT22_ERROR;
        delay_us(1);
    }

    __asm volatile ("cpsid i" : : : "memory");

    int8_t read_result = DHT22_OK;
    //READ 40 bits (5 BYTES0)
    for(int i = 0; i < 5; i++){
        for(int j = 0; j < 8; j++){
            timeout = 0;
            while(!(DHT22_PORT->IDR & (1 << DHT22_PIN))){
                if(++timeout > 1000){
                    return DHT22_ERROR;
                    break;
                }
                delay_us(1);
            } //wait until high

            if (read_result == DHT22_ERROR) break;
            
            delay_us(50);
            timeout = 0;
            if(DHT22_PORT->IDR & (1 << DHT22_PIN)){
                //send bit 1 into array, sensor send MSB first
                data[i] |= (1 << (7 - j));
                while(DHT22_PORT->IDR & (1 << DHT22_PIN)){
                    if(++timeout > 1000){
                        return DHT22_ERROR;
                        break;
                    }
                    delay_us(1);
                }
            }
            if (read_result == DHT22_ERROR) break;
        }
        if (read_result == DHT22_ERROR) break;
    }
    __asm volatile ("cpsie i" : : : "memory");

    if (read_result == DHT22_ERROR) return DHT22_ERROR;

    //CHECKSUM
    uint8_t Checksum = (uint8_t)(data[0] + data[1] + data[2] + data[3]);
    if (Checksum != data[4]) {
        return DHT22_ERROR;
    }

    //RESULT
    uint16_t rawHumidity = (data[0] << 8 | data[1]);
    uint16_t rawTemperature = (data[2] << 8 | data[3]);

     out_data->humidity = (float)rawHumidity / 10.0f;

    if(rawTemperature & 0x8000){ //1000 0000 0000 0000
        rawTemperature &= 0x7FFF; //Remove first bit
        out_data->temperature = (float)rawTemperature / -10.0f;
    }
    else{out_data->temperature = (float)rawTemperature / 10.0f;}

    return DHT22_OK;

}