#include "DHT22.h"
#include "Oled.h"

// --- Định nghĩa chân kết nối: PA5 ---
#define DHT22_PORT  GPIOA
#define DHT22_PIN   5

/*===============DELAYUS================*/

// Cấu hình TIM2 chạy ở tần số 1MHz (1 tick = 1 micro-giây)
static void TIM2_Init_us(void) {
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN; // Cấp Clock cho TIM2
    
    // Giả định hệ thống chạy ở 8MHz (HSI mặc định của STM32)
    // Prescaler = Clock / Tần_số_mong_muốn - 1 = 8000000 / 1000000 - 1 = 7
    TIM2->PSC = 8 - 1; 
    TIM2->ARR = 0xFFFF; // Đếm đến tối đa (65535)
    
    TIM2->CR1 |= TIM_CR1_CEN; // Khởi động đếm
}

static void delay_us(uint16_t us){
    TIM2->CNT = 0; //reset 
    while (TIM2->CNT < us); //truyen thoi gian vao us de dem
}
/*===============DELAY MS================*/
void delay_ms(uint32_t ms)
{
    TIM2_Init_us();
    while(ms--)
    {
        delay_us(1000);
    }
}
/*=============================*/
//Set chan tin hieu thanh Output Push-Pull  
static void Set_pin_output(void){
    DHT22_PORT->CRL &= ~(0xF << 20); //0xF = mask 1111, xoa bit PA5
    DHT22_PORT->CRL |=  (0x6 << 20); //0010, MODE5[1:0] = 10 (2Mhz), CNF5[1:0] = 00 (push-pull)
}

static void Set_pin_input(void){
    DHT22_PORT->CRL &= ~(0xF << 20);
    DHT22_PORT->CRL |=  (0x8 << 20); //1000; CNF5 = 10 (in pullup/down); MODE5 = 00
    DHT22_PORT->ODR |=  (1 << DHT22_PIN); //ODR 1 dien tro keo len (pullup)
}

/*=========================================== USER ==============================================*/

void DHT22_Init(void){
    //Cap clock
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;

    //Khoi tao us
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
    DHT22_PORT->BSRR = (1 << (DHT22_PIN + 16)); //Keo LOW
    delay_us(20000);

    DHT22_PORT->BSRR = (1 << DHT22_PIN); //Keo HIGH
    delay_us(30);

    Set_pin_input(); //set chan tin hieu thanh input de nhan response tu DHT22

    //WAIT FOR RESPONSE
    timeout = 0;
    while(DHT22_PORT->IDR & (1 << DHT22_PIN)){ // doi LOW
         if(++timeout > 1000) return DHT22_ERROR;
         delay_us(1);
    }
    

    timeout = 0;
    while(!(DHT22_PORT->IDR & (1 << DHT22_PIN))){ //doi HIGH
        if(++timeout > 1000) return DHT22_ERROR;
        delay_us(1);
    }

    timeout = 0;
    while(DHT22_PORT->IDR & (1 << DHT22_PIN)){ //LOW again
        if(++timeout > 1000) return DHT22_ERROR;
        delay_us(1);
    }

    //READ 40 bits (5 BYTES0)
    for(int i = 0; i < 5; i++){
        for(int j = 0; j < 8; j++){
            while(!(DHT22_PORT->IDR & (1 << DHT22_PIN))){
                if(++timeout > 1000)
                    return DHT22_ERROR;
            } //wait until high
            
            delay_us(50);
            if(DHT22_PORT->IDR & (1 << DHT22_PIN)){
                //send bit 1 into array, sensor send MSB first
                data[i] |= (1 << (7 - j));
                while(DHT22_PORT->IDR & (1 << DHT22_PIN)){
                    if(++timeout > 1000)
                        return DHT22_ERROR;
                }
            }
        }
    }

    //CHECKSUM
    uint8_t Checksum = (uint8_t)(data[0] + data[1] + data[2] + data[3]);
    if (Checksum != data[4]) {
        return DHT22_ERROR;
    }

    //RESULT
    uint16_t rawHumidity = (data[0] << 8 | data[1]); //ghep thanh 16 bit
    uint16_t rawTemperature = (data[2] << 8 | data[3]);//nguyen + thap phan

     out_data->humidity = (float)rawHumidity / 10.0f;

    // RESULT (ĐÃ SỬA CHO DHT11)
    //out_data->humidity = (float)data[0];
    //out_data->temperature = (float)data[2];

    if(rawTemperature & 0x8000){ //1000 0000 0000 0000
        rawTemperature &= 0x7FFF; //Bo bit dau
        out_data->temperature = (float)rawTemperature / -10.0f;
    }
    else{out_data->temperature = (float)rawTemperature / 10.0f;}

    return DHT22_OK;

}