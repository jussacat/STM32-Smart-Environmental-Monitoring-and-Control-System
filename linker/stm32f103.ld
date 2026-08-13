/* 
 * Kịch bản liên kết (Linker Script) cho STM32F103C8T6 
 * Flash: 64KB, RAM: 20KB
 */

/* Điểm vào của chương trình. Reset_Handler được định nghĩa trong file Startup */
ENTRY(Reset_Handler)

/* Tính toán đỉnh của Stack (Top of Stack). 
 * Nằm ở cuối RAM: 0x20000000 + 20KB (0x5000) = 0x20005000 
 */
_estack = 0x20005000;

/* Định nghĩa không gian bộ nhớ vật lý */
MEMORY
{
  FLASH (rx)      : ORIGIN = 0x08000000, LENGTH = 64K
  RAM (xrw)       : ORIGIN = 0x20000000, LENGTH = 20K
}

/* Định nghĩa cách sắp xếp các vùng (sections) vào bộ nhớ */
SECTIONS
{
  /* 1. Bảng vector ngắt (Interrupt Vector Table) 
   * BẮT BUỘC phải nằm ở đầu Flash (địa chỉ 0x08000000)
   */
  .isr_vector :
  {
    . = ALIGN(4);
    KEEP(*(.isr_vector)) /* KEEP để báo GCC không được tối ưu hóa xóa phần này */
    . = ALIGN(4);
  } >FLASH

  /* 2. Vùng Code (Chứa các lệnh thực thi) */
  .text :
  {
    . = ALIGN(4);
    *(.text)           /* Chứa code C của bạn */
    *(.text*)          
    . = ALIGN(4);
  } >FLASH

  /* 3. Vùng Dữ liệu chỉ đọc (Read-only data)
   * Chứa các hằng số (const) hoặc chuỗi ký tự ("Hello World")
   */
  .rodata :
  {
    . = ALIGN(4);
    *(.rodata)         
    *(.rodata*)        
    . = ALIGN(4);
  } >FLASH

  /* Lưu lại địa chỉ của vùng .data nằm trên Flash để tí nữa copy xuống RAM */
  _sidata = LOADADDR(.data);

  /* 4. Vùng Dữ liệu đã khởi tạo (.data)
   * Chứa các biến toàn cục có giá trị khởi tạo (VD: int count = 5;)
   * Vùng này sẽ được lưu ở FLASH nhưng khi chạy phải copy xuống RAM
   */
  .data : 
  {
    . = ALIGN(4);
    _sdata = .;        /* Đánh dấu điểm bắt đầu của .data trên RAM */
    *(.data)           
    *(.data*)          
    . = ALIGN(4);
    _edata = .;        /* Đánh dấu điểm kết thúc của .data trên RAM */
  } >RAM AT> FLASH     /* "Lưu ở FLASH, nhưng cấp phát địa chỉ ở RAM" */

  /* 5. Vùng Dữ liệu chưa khởi tạo (.bss)
   * Chứa biến toàn cục không khởi tạo (VD: int status;) hoặc gán bằng 0
   * Không chiếm dung lượng Flash, chỉ được cấp phát trên RAM
   */
  .bss :
  {
    . = ALIGN(4);
    _sbss = .;         /* Đánh dấu điểm bắt đầu vùng .bss */
    __bss_start__ = _sbss;
    *(.bss)
    *(.bss*)
    *(COMMON)
    . = ALIGN(4);
    _ebss = .;         /* Đánh dấu điểm kết thúc vùng .bss */
    __bss_end__ = _ebss;
  } >RAM
    _end = .;
  end = .;
}