PROJ        := lightstrip-mcu
TARGET_ELF  := $(PROJ).elf
BUILD_DIR   := ./bin
SRC_DIRS    := ./src

MCU_AVRDUDE := m328p
MCU_GCC     := atmega328p
USB_DEVICE  := /dev/ttyUSB0

INC         := /usr/avr/include

CC          := avr-gcc
LDFLAGS     :=
CFLAGS      := \
	-O3 \
	-MMD \
	-MP \
	-std=gnu99 \
	$(addprefix -I,$(INC)) \
	-mmcu=$(MCU_GCC) \
	-Wall \
	-Wextra \
	-Werror

SRCS := $(shell find $(SRC_DIRS) -name "*.c" -o -name "*.S")

OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:.o=.d)

all: $(BUILD_DIR)/$(TARGET_ELF)
	avr-size --mcu=$(MCU_GCC) -C $(BUILD_DIR)/$(TARGET_ELF)

flash: $(BUILD_DIR)/$(TARGET_ELF)
	avr-objcopy -O ihex $(BUILD_DIR)/$(TARGET_ELF) $(BUILD_DIR)/$(PROJ).hex
	avrdude -v -p $(MCU_AVRDUDE) -c arduino -P $(USB_DEVICE) -b 57600 -D -U flash:w:$(BUILD_DIR)/$(PROJ).hex:i

$(BUILD_DIR)/$(TARGET_ELF): $(OBJS)
	@$(CC) -mmcu=$(MCU_GCC) $(OBJS) -o $@ $(LDFLAGS)
	@echo Done. Placed elf at $(BUILD_DIR)/$(TARGET_ELF)

$(BUILD_DIR)/%.c.o: %.c
	@mkdir -p $(dir $@)
	@echo Compiling $<
	@$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/%.S.o: %.S
	@mkdir -p $(dir $@)
	@echo Compiling $<
	@$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean
clean:
	$(RM) -r ./bin

-include $(DEPS)
