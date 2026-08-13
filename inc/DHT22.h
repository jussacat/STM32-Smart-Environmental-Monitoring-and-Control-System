#ifndef DHT22_H
#define DHT22_H

#include "stm32f10x.h" // Thư viện CMSIS cốt lõi

// Trạng thái trả về của cảm biến
#define DHT22_OK        0
#define DHT22_ERROR     -1

// Cấu trúc chứa dữ liệu môi trường
typedef struct {
    float temperature;
    float humidity;
} DHT22_Data_t;

// Khởi tạo phần cứng (Timer và GPIO) cho DHT22
void DHT22_Init(void);

// Hàm đọc dữ liệu từ cảm biến
// Trả về DHT22_OK (0) nếu thành công, trả về DHT22_ERROR (-1) nếu mất kết nối hoặc sai mã checksum
int8_t DHT22_Read(DHT22_Data_t *out_data);
void delay_ms(uint32_t ms);
#endif /* DHT22_H */