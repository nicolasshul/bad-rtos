PROJECT := my-rtos

BUILD_DIR := build

# Toolchain Setup

CC := arm-none-eabi-gcc
AS := arm-none-eabi-gcc
OBJCOPY := arm-none-eabi-objcopy
SIZE := arm-none-eabi-size
GDB := arm-none-eabi-gdb

# Add ability to change later
BOARD_TYPE := cortex-m4

BOARD_DIR := boards/nucleo_l432kc
LINKER_SCRIPT := $(BOARD_DIR)/linker/STM32L432KCUX_FLASH.ld
STARTUP := $(BOARD_DIR)/startup/startup_stm32l432xx.s

# MCU Flags

CPU := -mcpu=cortex-m4
FPU := -mfpu=fpv4-sp-d16
FLOAT-ABI := -mfloat-abi=hard
MCU := $(CPU) -mthumb -mthumb-interwork $(FPU) $(FLOAT-ABI)

# Build dir mkdir macro

MKDIR_CMD := mkdir -p "$(BUILD_DIR)"

# Defines

DEFINES := -DSTM32L432xx
 
# Includes

INCLUDES := \
	-Iapp/inc \
	-Ikernel/inc \
	-Idrivers/inc \
	-Iports/cortex-m4/inc \
	-I$(BOARD_DIR)/inc \
	-Iutils/inc \
	-Ithird-party/cmsis/Core/Include \
	-Ithird-party/cmsis/Device/ST/STM32L4xx/Include \
	-I$(BOARD_DIR)/drivers/STM32L4xx_HAL_Driver/Inc \

# Sources

HAL_DIR := $(BOARD_DIR)/drivers/STM32L4xx_HAL_Driver/Src

HAL_SOURCES := \
	$(HAL_DIR)/stm32l4xx_hal_tim.c \
	$(HAL_DIR)/stm32l4xx_hal_tim_ex.c \
	$(HAL_DIR)/stm32l4xx_hal.c \
	$(HAL_DIR)/stm32l4xx_hal_rcc.c \
	$(HAL_DIR)/stm32l4xx_hal_rcc_ex.c \
	$(HAL_DIR)/stm32l4xx_hal_flash.c \
	$(HAL_DIR)/stm32l4xx_hal_flash_ex.c \
	$(HAL_DIR)/stm32l4xx_hal_flash_ramfunc.c \
	$(HAL_DIR)/stm32l4xx_hal_gpio.c \
	$(HAL_DIR)/stm32l4xx_hal_i2c.c \
	$(HAL_DIR)/stm32l4xx_hal_i2c_ex.c \
	$(HAL_DIR)/stm32l4xx_hal_dma.c \
	$(HAL_DIR)/stm32l4xx_hal_dma_ex.c \
	$(HAL_DIR)/stm32l4xx_hal_pwr.c \
	$(HAL_DIR)/stm32l4xx_hal_pwr_ex.c \
	$(HAL_DIR)/stm32l4xx_hal_cortex.c \
	$(HAL_DIR)/stm32l4xx_hal_exti.c \

C_SOURCES := \
	$(shell find app/src kernel/src drivers/src ports/$(BOARD_TYPE)/src $(BOARD_DIR)/src utils/src -name '*.c') \
	$(BOARD_DIR)/startup/system_stm32l4xx.c \
	$(HAL_SOURCES) \

ASM_SOURCES := \
	$(STARTUP) \
	ports/$(BOARD_TYPE)/src/port_asm.s

# Output Files

ELF := $(BUILD_DIR)/$(PROJECT).elf
BIN := $(BUILD_DIR)/$(PROJECT).bin
HEX := $(BUILD_DIR)/$(PROJECT).hex
MAP := $(BUILD_DIR)/$(PROJECT).map

# Object Files

C_OBJECTS := $(patsubst %.c,$(BUILD_DIR)/%.o,$(C_SOURCES))
ASM_OBJECTS := $(patsubst %.s,$(BUILD_DIR)/%.o,$(ASM_SOURCES))
OBJECTS := $(C_OBJECTS) $(ASM_OBJECTS)

# Dependency Files

DEPS := $(OBJECTS:.o=.d)

# Flags

COMMON_FLAGS := \
	$(MCU) \
	$(DEFINES) \
	$(INCLUDES) \
	-ffreestanding \
	-fdata-sections \
	-ffunction-sections \
	-Wall \
	-Wextra \
	-Werror \
	-g3 \
	-O0

CFLAGS := \
	$(COMMON_FLAGS) \
	-std=c11 \
	-MMD \
	-MP

ASFLAGS := \
	$(COMMON_FLAGS) \
	-x assembler-with-cpp \
	-MMD \
	-MP

LDFLAGS := \
	$(MCU) \
	-T$(LINKER_SCRIPT) \
	-Wl,-Map=$(MAP),--cref \
	-Wl,--gc-sections \
	-Wl,--print-memory-usage \
	--specs=nano.specs \
	-lc -lm -lnosys

# Targets

.PHONY: all clean flash debug gdb size

all: $(ELF) $(BIN) $(HEX) size

$(ELF): $(OBJECTS)
	@mkdir -p $(dir $@)
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@

$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: %.s
	@mkdir -p $(dir $@)
	$(AS) $(ASFLAGS) -c $< -o $@

$(BIN): $(ELF)
	$(OBJCOPY) -O binary $< $@

$(HEX): $(ELF)
	$(OBJCOPY) -O ihex $< $@

size: $(ELF)
	$(SIZE) $<

flash: $(ELF)
	openocd \
		-f interface/stlink.cfg \
		-f target/stm32l4x.cfg \
		-c "program $(ELF) verify reset exit"

debug:
	openocd \
		-f interface/stlink.cfg \
		-f target/stm32l4x.cfg

gdb: $(ELF)
	$(GDB) -ex "target remote localhost:3333" -ex "monitor reset halt" -ex "load" $(ELF)

clean:
	rm -rf $(BUILD_DIR) compile_flags.txt

-include $(DEPS)
