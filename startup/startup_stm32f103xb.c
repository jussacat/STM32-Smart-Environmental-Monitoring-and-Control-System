#include <stdint.h>
#include <string.h>

// symbols provided by the .ld linker script file
extern uint32_t _etext;
extern uint32_t _sdata;
extern uint32_t _edata;
extern uint32_t _sidata;
extern uint32_t _sbss;
extern uint32_t _ebss;
extern uint32_t _estack;

extern int main(void);

#pragma GCC optimize("O3")

void Default_Handler(void) {
    while(1);
}

void NMI_Handler(void)        __attribute__((weak, alias("Default_Handler")));
void HardFault_Handler(void)  __attribute__((weak, alias("Default_Handler")));
void MemManage_Handler(void)  __attribute__((weak, alias("Default_Handler")));
void BusFault_Handler(void)   __attribute__((weak, alias("Default_Handler")));
void UsageFault_Handler(void) __attribute__((weak, alias("Default_Handler")));
void SVC_Handler(void)        __attribute__((weak, alias("Default_Handler")));
void DebugMon_Handler(void)   __attribute__((weak, alias("Default_Handler")));
void PendSV_Handler(void)     __attribute__((weak, alias("Default_Handler")));
void SysTick_Handler(void)    __attribute__((weak, alias("Default_Handler")));

__attribute__ ((naked)) 
void Reset_Handler(void)
{
 // copy initialized global variables in .data from flash to SRAM
 memcpy(&_sdata, &_sidata, (size_t)&_edata - (size_t)&_sdata);

 // init uninitialized global variables in .bss with zero
 memset(&_sbss, 0, (size_t)&_ebss - (size_t)&_sbss);
 
 // now invoke main()
 main();

 // if main() returns:
 for (;;)
 ;
}

__attribute__((section(".isr_vector")))
void (* const vectors[])(void) = 
{                                                                                                                                                                                
    (void (*)(void))(&_estack),  // 0: Initial Stack Pointer 
    Reset_Handler,               // 1: Reset 
    NMI_Handler,                 // 2: NMI
    HardFault_Handler,           // 3: HardFault 
    MemManage_Handler,           // 4: MPU Fault 
    BusFault_Handler,            // 5: Bus Fault
    UsageFault_Handler,          // 6: Usage Fault
    0, 0, 0, 0,                  // 7-10: Reserved
    SVC_Handler,                 // 11: SVCall (FreeRTOS)
    DebugMon_Handler,            // 12: Debug Monitor
    0,                           // 13: Reserved
    PendSV_Handler,              // 14: PendSV (FreeRTOS)
    SysTick_Handler,             // 15: SysTick (FreeRTOS)
};