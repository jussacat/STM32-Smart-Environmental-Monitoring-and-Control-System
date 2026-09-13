    # STM32 Bare-Metal Environmental Monitoring System with FreeRTOS                                                                                                                 
                                                                                                                                                                                     
    A robust, lightweight, and deterministic embedded system built on the **STM32F103C8T6 (ARM Cortex-M3)** microcontroller. This project demonstrates bare-metal firmware           
  development from scratch—**zero HAL/SPL dependencies**—integrated with the **FreeRTOS Real-Time Operating System** kernel.                                                         
                                                                                                                                                                                     
    ![Language](https://img.shields.io/badge/Language-C-blue.svg)                                                                                                                    
    ![Architecture](https://img.shields.io/badge/Architecture-ARM%20Cortex--M3-green.svg)                                                                                            
    ![OS](https://img.shields.io/badge/RTOS-FreeRTOS%20v10.x-orange.svg)                                                                                                             
    ![Toolchain](https://img.shields.io/badge/Toolchain-arm--none--eabi--gcc-lightgrey.svg)                                                                                          
                                                                                                                                                                                     
    ---                                                                                                                                                                              
                                                                                                                                                                                     
    ## Key Highlights                                                                                                                                                             
                                                                                                                                                                                     
    - **Pure Bare-Metal Drivers:** All peripheral drivers (USART1, I2C1, I2C2, TIM2, GPIO) are implemented via direct memory-mapped register access.                                 
    - **Custom Startup & Linker Script:** Custom `stm32f103.ld` and `startup_stm32f103xb.c` handling system memory allocation (`.text`, `.data`, `.bss`, Stack) and interrupt vector 
  routing.                                                                                                                                                                           
    - **Native FreeRTOS Porting:** Direct integration of FreeRTOS CM3 port (`port.c`, `heap_4.c`), properly binding `SVC_Handler`, `PendSV_Handler`, and `SysTick_Handler` in the    
  vector table.                                                                                                                                                                      
    - **Concurrency & Resource Management:**                                                                                                                                         
      - **Mutex Protection:** Safeguards shared OLED (I2C1) and UART1 transmission against race conditions between tasks.                                                            
      - **Critical Section Protection:** Guarantees microsecond-accurate 1-Wire bit-banging for the DHT22 sensor, immune to RTOS preemption and jitter.                              
                                                                                                                                                                                     
    ---                                                                                                                                                                              
                                                                                                                                                                                     
    ## Hardware Architecture & Pinout                                                                                                                                              
                                                                                                                                                                                     
    | Peripheral | Hardware / Protocol | STM32 Pin | Note |                                                                                                                          
    | :--- | :--- | :--- | :--- |                                                                                                                                                    
    | **OLED 0.96"** | SSD1306 (I2C1) | `PB6` (SCL), `PB7` (SDA) | Dedicated I2C1 Bus |                                                                                              
    | **RTC Module** | DS3231 / DS1307 (I2C2) | `PB10` (SCL), `PB11` (SDA) | Independent I2C2 Bus |                                                                                  
    | **Temp/Humidity** | DHT22 / AM2302 (1-Wire) | `PA5` | Microsecond delay via TIM2 |                                                                                             
    | **Serial Debug** | USART1 (9600 bps, 8N1) | `PA9` (TX), `PA10` (RX) | Connect to USB-UART converter |                                                                          
    | **Debugger** | SWD | `PA13` (SWDIO), `PA14` (SWCLK) | ST-Link V2 |                                                                                                             
                                                                                                                                                                                     
    ---                                                                                                                                                                              
                                                                                                                                                                                     
    ## Software Architecture                                                                                                                                                      
                                                                                                                                                                                     
    The application runs a preemptive, priority-based FreeRTOS scheduler:                                                                                                            
                                                                                                                                                                                     
    ```text                                                                                                                                                                          
    +-------------------------------------------------------------+                                                                                                                  
    |                      FreeRTOS Scheduler                     |                                                                                                                  
    +-------------------------------------------------------------+                                                                                                                  
               |                                       |                                                                                                                             
        [Priority 2 (High)]                     [Priority 1 (Low)]                                                                                                                   
         vTask_RTC (Period: 1s)                  vTask_DHT22 (Period: 2.5s)                                                                                                          
               |                                       |                                                                                                                             
        - Read RTC over I2C2                    - Critical Section: Read DHT22 (1-Wire)                                                                                              
        - Take Mutex                            - Take Mutex                                                                                                                         
        - Render Time to OLED                   - Render Temp/Humid to OLED                                                                                                          
        - Transmit Time over UART               - Transmit Data over UART                                                                                                            
        - Give Mutex                            - Give Mutex                                                                                                                         
                                                                                                                                                                                     
  ### Memory Footprint                                                                                                                                                               
                                                                                                                                                                                     
  • Flash: ~14.1 KB / 64 KB (22%)                                                                                                                                                    
  • RAM: ~13.0 KB / 20 KB (Includes 12 KB FreeRTOS Dynamic Heap)                                                                                                                     
  • Main System Stack (MSP): >7 KB free headroom                                                                                                                                     
  ──────                                                                                                                                                                             
  ## Project Structure                                                                                                                                                            
                                                                                                                                                                                     
    ├── app/                                                                                                                                                                         
    │   └── main.c             # System entry point                                                                                                                                  
    ├── inc/                   # Application & Driver header files                                                                                                                   
    │   ├── FreeRTOSConfig.h   # FreeRTOS kernel configuration                                                                                                                       
    │   ├── task_manager.h     # Task declarations & OS startup                                                                                                                      
    │   ├── display.h          # Display & UI handlers                                                                                                                               
    │   ├── DHT22.h            # 1-Wire sensor driver                                                                                                                                
    │   ├── Oled.h             # SSD1306 OLED driver                                                                                                                                 
    │   ├── RTC.h              # DS3231/DS1307 driver                                                                                                                                
    │   └── uart.h             # USART1 driver                                                                                                                                       
    ├── src/                   # Driver and logic implementations                                                                                                                    
    ├── cmsis/                 # Register mapping definitions (stm32f10x.h)                                                                                                          
    ├── startup/               # Reset_Handler and Vector Table
    ├── linker/                # Memory layout (stm32f103.ld)
    ├── FreeRTOS/              # FreeRTOS Kernel source files
    └── Makefile               # Build & Flash automation
  ──────
  ## Getting Started
  
  ### Prerequisites
  
  • Toolchain: arm-none-eabi-gcc
  • Flashing Utility: stlink-tools (or OpenOCD)
  • Serial Monitor: screen, minicom, or VSCode Serial Monitor
  
  ### Build & Flash
  
  1. Clone the repository:
    git clone https://github.com/<your-username>/<repo-name>.git
    cd <repo-name>
  
  2. Compile the project:
    make clean && make
  
  3. Flash to STM32 via ST-Link:
    make flash
  
  4. Monitor UART output (9600 Baud):
    screen /dev/ttyUSB0 9600
