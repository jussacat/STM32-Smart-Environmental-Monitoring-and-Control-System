# STM32 Bare-Metal Real-Time Environmental Monitoring System

A deterministic, resource-optimized firmware architecture developed for the STM32F103C8T6 microcontroller (ARM Cortex-M3). The system implements bare-metal hardware drivers through direct memory-mapped register manipulation—completely free of HAL or Standard Peripheral Libraries—integrated with the FreeRTOS real-time kernel.

---

## Executive Summary

Standard embedded firmware implementations frequently rely on vendor-provided hardware abstraction layers (HAL) and code generators, introducing substantial flash/RAM overhead and non-deterministic execution paths. This project demonstrates low-level systems engineering by building the hardware abstraction layer, system startup sequence, memory layout, and real-time scheduling primitives directly from hardware registers and architectural specifications.

---

## Technical Specifications

- **Target Device:** STM32F103C8T6
- **Core Architecture:** ARM 32-bit Cortex-M3
- **Instruction Set:** Thumb-2
- **System Clock:** 8.0 MHz (HSI default)
- **Operating System:** FreeRTOS Kernel v10.x
- **Scheduling Policy:** Preemptive, Priority-based with Time Slicing
- **Memory Management:** Dynamic Heap Scheme 4 (`heap_4.c`)
- **Toolchain:** GNU Arm Embedded Toolchain (`arm-none-eabi-gcc` 13.2+)
- **Build System:** GNU Make

---

## Key Engineering Features

### 1. Register-Level Peripheral Drivers
All peripheral controllers are driven through custom structure representations mapped directly to base physical addresses:
- **USART1:** Configured for 8N1 transmission at 9600 bps; fractional baud rate calculation executed via `USART_BRR`.
- **I2C1 & I2C2:** Independent dual-bus architecture separating OLED display operations (I2C1, Standard Mode 100 kHz) from real-time clock communications (I2C2, Standard Mode 100 kHz) to eliminate bus contention.
- **General-Purpose Timer (TIM2):** Configured with an 8 MHz input clock and prescaler `PSC = 7` to yield a 1.0 MHz counter tick (1.0 microsecond resolution) for protocol pulse timing.
- **GPIO Driver:** Pin multiplexing, pull-up/pull-down control, and alternate function routing configured via direct manipulation of `CRL` and `CRH` registers.

### 2. Custom Startup Sequence and Vector Table
Rather than using generic vendor startup assemblies, the vector table and system bootstrap are implemented in C (`startup_stm32f103xb.c`):
- Exception vectors are aligned and assigned to the `.isr_vector` ELF section mapped to Flash base address `0x08000000`.
- System exception vectors (SVC, PendSV, SysTick) are weakly aliased to default handlers and bound at link time to FreeRTOS port implementations (`SVC_Handler`, `PendSV_Handler`, `SysTick_Handler`).
- `Reset_Handler` handles SRAM initialization: copies `.data` from non-volatile Flash storage (`LOADADDR`) into SRAM, zeroes out the uninitialized `.bss` section, and invokes `main()`.

### 3. Linker Memory Layout (`stm32f103.ld`)
The memory map enforces strict boundary partitions:
- **Flash Memory:** 64 KB (`0x08000000` - `0x0800FFFF`)
- **SRAM:** 20 KB (`0x20000000` - `0x20004FFF`)
- **Initial Stack Pointer (`_estack`):** Assigned to top of RAM at `0x20005000`.

### 4. Concurrency and Resource Synchronization
- **Hardware Mutex (`xHardwareMutex`):** Resolves resource contention over the shared OLED display (I2C1) and serial debug terminal (USART1), preventing data interleaving between competing tasks.
- **Critical Section Guarding:** Microsecond-precision pulse measurement for the DHT22 1-Wire protocol is protected using `cpsid i` and `cpsie i` assembler instructions. This prevents FreeRTOS context switches or SysTick interrupts from distorting pulse-width timing and triggering checksum failures.

---

## Hardware Interfacing

| Subsystem | Component | Interface | Microcontroller Pins | Function |
| :--- | :--- | :--- | :--- | :--- |
| Core MCU | STM32F103C8T6 | Direct | VDD, VSS, NRST | Processing Core |
| Display | SSD1306 0.96" OLED | I2C1 | PB6 (SCL), PB7 (SDA) | User Interface (128x64) |
| Real-Time Clock | DS3231 / DS1307 | I2C2 | PB10 (SCL), PB11 (SDA) | Timestamping & Calendar |
| Environment Sensor | DHT22 / AM2302 | 1-Wire (GPIO) | PA5 | Temperature & Humidity |
| Serial Terminal | CP2102 / CH340 | USART1 | PA9 (TX), PA10 (RX) | Telemetry & Debugging |
| Debug & Flash | ST-Link V2 | SWD | PA13 (SWDIO), PA14 (SWCLK) | Flashing & Trace |

---

## Real-Time Task Architecture

The real-time application layer consists of two independent, statically bounded periodic tasks managed by the FreeRTOS preemptive kernel:

```text
+-------------------------------------------------------------------------+
|                           FreeRTOS Kernel                               |
|                  (SysTick Interval: 1.0 ms)                             |
+-------------------------------------------------------------------------+
        |                                                 |
Task: vTask_RTC (Priority: 2)                    Task: vTask_DHT22 (Priority: 1)
Period: 1000 ms                                  Period: 2500 ms
Stack: 256 words (1024 B)                        Stack: 256 words (1024 B)
        |                                                 |
1. Read RTC Registers (I2C2)                     1. Enter Critical Section
2. Acquire xHardwareMutex                        2. Sample 40-Bit 1-Wire Signal (PA5)
3. Update Time String on OLED                    3. Exit Critical Section
4. Transmit Telemetry over USART1                4. Acquire xHardwareMutex
5. Release xHardwareMutex                        5. Format & Render Data to OLED
6. Block on vTaskDelay(1000)                     6. Transmit Telemetry over USART1
                                                 7. Release xHardwareMutex
                                                 8. Block on vTaskDelay(2500)
## Real-Time Task Architecture

The real-time application layer consists of two independent, statically bounded periodic tasks managed by the FreeRTOS preemptive kernel:

```text
+-------------------------------------------------------------------------+
|                           FreeRTOS Kernel                               |
|                  (SysTick Interval: 1.0 ms)                             |
+-------------------------------------------------------------------------+
        |                                                 |
Task: vTask_RTC (Priority: 2)                    Task: vTask_DHT22 (Priority: 1)
Period: 1000 ms                                  Period: 2500 ms
Stack: 256 words (1024 B)                        Stack: 256 words (1024 B)
        |                                                 |
1. Read RTC Registers (I2C2)                     1. Enter Critical Section
2. Acquire xHardwareMutex                        2. Sample 40-Bit 1-Wire Signal (PA5)
3. Update Time String on OLED                    3. Exit Critical Section
4. Transmit Telemetry over USART1                4. Acquire xHardwareMutex
5. Release xHardwareMutex                        5. Format & Render Data to OLED
6. Block on vTaskDelay(1000)                     6. Transmit Telemetry over USART1
                                                 7. Release xHardwareMutex
                                                 8. Block on vTaskDelay(2500)
```

### Memory Footprint Analysis

```text
Memory Section                                │ Allocated Size                │ Device Capacity            │ Utilization
──────────────────────────────────────────────┼───────────────────────────────┼───────────────────────────┼────────────
Flash Memory (.text + .rodata + .data)        │ 14,156 Bytes                  │ 65,536 Bytes (64 KB)     │ 21.6%
SRAM (.data + .bss)                           │ 13,056 Bytes                  │ 20,480 Bytes (20 KB)     │ 63.7%
FreeRTOS Dynamic Heap (configTOTAL_HEAP_SIZE) │ 12,288 Bytes (12 KB)          │ Allocated in .bss         │ N/A
Available MSP Stack Headroom                  │ 7,424 Bytes (~7.25 KB)        │ Upper SRAM region         │ N/A
```

## Directory Structure

```text
.
├── Makefile
├── linker/
│   └── stm32f103.ld
├── startup/
│   └── startup_stm32f103xb.c
├── cmsis/
│   └── stm32f10x.h
├── inc/
│   ├── FreeRTOSConfig.h
│   ├── task_manager.h
│   ├── display.h
│   ├── DHT22.h
│   ├── Oled.h
│   ├── RTC.h
│   └── uart.h
├── src/
│   ├── task_manager.c
│   ├── display.c
│   ├── DHT22.c
│   ├── Oled.c
│   ├── RTC.c
│   └── uart.c
├── app/
│   └── main.c
└── FreeRTOS/
```

## Build and Deployment Procedure

### Prerequisites

Ensure the following tools are installed in the host development environment:

- GNU Arm Embedded Toolchain (`arm-none-eabi-gcc`, `arm-none-eabi-objcopy`, `arm-none-eabi-size`)
- ST-Link utilities (`stlink-tools`) or OpenOCD
- Terminal emulator (`screen`, `minicom`, or standard serial console)

### Compilation

Execute the default build target using GNU Make:

```bash
make clean && make
```

Successful compilation outputs memory section sizing alongside generated binary artifacts in `build/`:

- `build/Envi_Buzzer.elf`: Executable and Linkable Format file containing debug symbols.
- `build/Envi_Buzzer.bin`: Raw binary file for non-volatile memory programming.
- `build/Envi_Buzzer.hex`: Intel HEX format representation.

### Firmware Programming

Connect an ST-Link V2 programmer to the target SWD port (PA13, PA14, GND, 3.3V) and flash the target:

```bash
make flash
```

### Telemetry Verification

Connect an external USB-to-UART bridge to PA9 (STM32 TX -> Bridge RX) and establish serial communication:

```bash
screen /dev/ttyUSB0 9600
```

