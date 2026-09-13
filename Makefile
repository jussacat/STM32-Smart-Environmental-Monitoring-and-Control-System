TARGET = Envi_Buzzer
BUILD_DIR = build

PREFIX = arm-none-eabi-
CC = $(PREFIX)gcc
AS = $(PREFIX)gcc -x assembler-with-cpp
CP = $(PREFIX)objcopy
SZ = $(PREFIX)size

C_SOURCES =  \
app/main.c \
src/DHT22.c \
src/Oled.c \
src/RTC.c \
src/uart.c \
src/display.c \
src/task_manager.c \
startup/startup_stm32f103xb.c \
FreeRTOS/tasks.c \
FreeRTOS/timers.c \
FreeRTOS/queue.c \
FreeRTOS/list.c \
FreeRTOS/event_groups.c \
FreeRTOS/portable/GCC/ARM_CM3/port.c \
FreeRTOS/portable/MemMang/heap_4.c

ASM_SOURCES =  

C_INCLUDES =  \
-Iinc \
-Icmsis \
-IFreeRTOS/include \
-IFreeRTOS/portable/GCC/ARM_CM3

LDSCRIPT = linker/stm32f103.ld

CPU = -mcpu=cortex-m3
MCU = $(CPU) -mthumb


OPT = -Os

CFLAGS = $(MCU) $(C_INCLUDES) $(OPT) -Wall -Wextra
CFLAGS += -ffunction-sections -fdata-sections 
CFLAGS += -MMD -MP -MF"$(@:%.o=%.d)" 

ASFLAGS = $(MCU) $(OPT) -Wall -fdata-sections -ffunction-sections


LDFLAGS = $(MCU) \
-T$(LDSCRIPT) \
--specs=nano.specs \
--specs=nosys.specs \
-Wl,-Map=$(BUILD_DIR)/$(TARGET).map,--cref \
-Wl,--gc-sections

# BUILD RULES
OBJECTS = $(addprefix $(BUILD_DIR)/,$(notdir $(C_SOURCES:.c=.o)))
vpath %.c $(sort $(dir $(C_SOURCES)))


# make
all: $(BUILD_DIR)/$(TARGET).elf $(BUILD_DIR)/$(TARGET).bin $(BUILD_DIR)/$(TARGET).hex print_size

# Link .o to .elf
$(BUILD_DIR)/$(TARGET).elf: $(OBJECTS) | $(BUILD_DIR)
	@echo "[LINKING] $@"
	@$(CC) $(OBJECTS) $(LDFLAGS) -o $@

#(.c) to (.o)
$(BUILD_DIR)/%.o: %.c Makefile | $(BUILD_DIR)
	@echo "[CC] $<"
	@$(CC) -c $(CFLAGS) $< -o $@

#(.s) to (.o)
$(BUILD_DIR)/%.o: %.s Makefile | $(BUILD_DIR)
	@echo "[AS] $<"
	@$(AS) -c $(ASFLAGS) $< -o $@

# Make .bin file
$(BUILD_DIR)/%.bin: $(BUILD_DIR)/%.elf | $(BUILD_DIR)
	@echo "[COPY] $@"
	@$(CP) -O binary -S $< $@

# Make hex. file
$(BUILD_DIR)/%.hex: $(BUILD_DIR)/%.elf | $(BUILD_DIR)
	@echo "[COPY] $@"
	@$(CP) -O ihex $< $@

# Make build folder
$(BUILD_DIR):
	@mkdir -p $@

#RAM & Flash
print_size: $(BUILD_DIR)/$(TARGET).elf
	@echo "\n--- MEMORY ---"
	@$(SZ) $<


# Clear build folder
clean:
	@echo "Clearing folders $(BUILD_DIR)..."
	@rm -rf $(BUILD_DIR)

flash: all
	@echo "Dang nap..."
	st-flash write $(BUILD_DIR)/$(TARGET).bin 0x8000000

# Include dependencies files
-include $(wildcard $(BUILD_DIR)/*.d)