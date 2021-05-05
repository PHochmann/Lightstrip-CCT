# Depending on clone, you need different baud rates:
# 2400
# 57600
# 115200

PROJ        := lightstrip-cct-receiver
TARGET_ELF  := $(PROJ).elf
BUILD_DIR   := ./bin
SRC_DIRS    := . ../common

FLASH_BAUDRATE := 115200
LOG_BAUDRATE   := 500000
F_CPU          := 16000000L

MCU_AVRDUDE    := m328p
MCU_GCC        := atmega328p
USB_DEVICE     := /dev/ttyUSB2

INC         := /usr/avr/include

CC          := avr-gcc
LDFLAGS     :=
CFLAGS      := \
	-O3 \
	-MMD \
	-MP \
	-std=gnu99 \
	-DF_CPU=$(F_CPU) \
	$(addprefix -I,$(INC)) \
	-mmcu=$(MCU_GCC) \
	-Wall \
	-Wextra \
	-Werror

SRCS := $(shell find $(SRC_DIRS) -name "*.c")

OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:.o=.d)

all: $(BUILD_DIR)/$(TARGET_ELF)
	avr-size --mcu=$(MCU_GCC) -C $(BUILD_DIR)/$(TARGET_ELF)

flash: all
	avr-objcopy -O ihex $(BUILD_DIR)/$(TARGET_ELF) $(BUILD_DIR)/$(PROJ).hex
	avrdude -v -p $(MCU_AVRDUDE) -c arduino -P $(USB_DEVICE) -b $(FLASH_BAUDRATE) -D -U flash:w:$(BUILD_DIR)/$(PROJ).hex:i

debug: flash
	python ../Logger.py $(USB_DEVICE) $(LOG_BAUDRATE)

$(BUILD_DIR)/$(TARGET_ELF): $(OBJS)
	@$(CC) -mmcu=$(MCU_GCC) $(subst ../,,$(OBJS)) -o $@ $(LDFLAGS)
	@echo Done. Placed elf at $(BUILD_DIR)/$(TARGET_ELF)

$(BUILD_DIR)/%.c.o: %.c
	@mkdir -p $(dir $(subst ../,,$@))
	@echo Compiling $<
	@$(CC) $(CFLAGS) -c $< -o $(subst ../,,$@)

.PHONY: clean
clean:
	$(RM) -r ./bin

-include $(DEPS)
