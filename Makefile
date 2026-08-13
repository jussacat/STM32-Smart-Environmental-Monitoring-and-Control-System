TARGET = Envi_Buzzer
BUILD_DIR = build

PREFIX = arm-none-eabi-
CC = $(PREFIX)gcc
AS = $(PREFIX)gcc -x assembler-with-cpp
CP = $(PREFIX)objcopy
SZ = $(PREFIX)size


# Khai báo các file C (Thêm file mới vào đây)
C_SOURCES =  \
app/main.c \
src/DHT22.c \
src/Oled.c \
src/RTC.c \
startup/startup_stm32f103xb.c

# Khai báo file Assembly (Startup script)
ASM_SOURCES =  

# Đường dẫn chứa các file header (.h)
C_INCLUDES =  \
-Iinc \
-Icmsis

# File linker script
LDSCRIPT = linker/stm32f103.ld


# Kiến trúc vi điều khiển (Cortex-M3)
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

# ==============================================================================
# BUILD RULES)
# ==============================================================================
# Xử lý danh sách file Object (.o) sẽ được đưa vào thư mục build/
OBJECTS = $(addprefix $(BUILD_DIR)/,$(notdir $(C_SOURCES:.c=.o)))
vpath %.c $(sort $(dir $(C_SOURCES)))


# Lệnh mặc định khi gõ "make"
all: $(BUILD_DIR)/$(TARGET).elf $(BUILD_DIR)/$(TARGET).bin $(BUILD_DIR)/$(TARGET).hex print_size

# Liên kết (Link) các file .o thành .elf
$(BUILD_DIR)/$(TARGET).elf: $(OBJECTS) | $(BUILD_DIR)
	@echo "[LINKING] $@"
	@$(CC) $(OBJECTS) $(LDFLAGS) -o $@

# Biên dịch (.c) sang (.o)
$(BUILD_DIR)/%.o: %.c Makefile | $(BUILD_DIR)
	@echo "[CC] $<"
	@$(CC) -c $(CFLAGS) $< -o $@

# Biên dịch (.s) sang (.o)
$(BUILD_DIR)/%.o: %.s Makefile | $(BUILD_DIR)
	@echo "[AS] $<"
	@$(AS) -c $(ASFLAGS) $< -o $@

# Tạo file .bin (Dùng để nạp thẳng vào bộ nhớ gốc)
$(BUILD_DIR)/%.bin: $(BUILD_DIR)/%.elf | $(BUILD_DIR)
	@echo "[COPY] $@"
	@$(CP) -O binary -S $< $@

# Tạo file .hex (Dùng cho một số phần mềm nạp khác)
$(BUILD_DIR)/%.hex: $(BUILD_DIR)/%.elf | $(BUILD_DIR)
	@echo "[COPY] $@"
	@$(CP) -O ihex $< $@

# Tạo thư mục build nếu chưa tồn tại
$(BUILD_DIR):
	@mkdir -p $@

# In ra thống kê dung lượng RAM và Flash
print_size: $(BUILD_DIR)/$(TARGET).elf
	@echo "\n--- MEMORY ---"
	@$(SZ) $<


# Xóa toàn bộ file đã build
clean:
	@echo "Clearing folders $(BUILD_DIR)..."
	@rm -rf $(BUILD_DIR)

# Nạp code tự động qua ST-Link (Chỉ chạy được nếu máy đã cài stlink-tools)
flash: all
	@echo "Dang nap..."
	st-flash write $(BUILD_DIR)/$(TARGET).bin 0x8000000

# Include các file dependencies tự sinh
-include $(wildcard $(BUILD_DIR)/*.d)