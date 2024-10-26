MCU			= atmega32u4
F_CPU		= 16000000

SRC_DIR		= src
INC_DIR		= include
BUILD_DIR	= build

C_SOURCES    = $(wildcard $(SRC_DIR)/*.c) $(wildcard $(SRC_DIR)/**/*.c)

CC			= avr-gcc
OBJCOPY		= avr-objcopy
CFLAGS		= -mmcu=$(MCU) -DF_CPU=$(F_CPU) -std=c11 -Os -Wall -Wextra -I$(INC_DIR)

TARGET		= simp-firmware


all: $(BUILD_DIR)/$(TARGET).hex

$(BUILD_DIR)/$(TARGET).hex:	$(BUILD_DIR)/$(TARGET).o
	$(OBJCOPY) -O ihex -R .eeprom $< $@

$(BUILD_DIR)/$(TARGET).o: $(C_SOURCES) | $(BUILD_DIR)
	$(CC) $(CFLAGS) $^ -o $@

$(BUILD_DIR):
	mkdir $@

flash: $(BUILD_DIR)/$(TARGET).hex
	sudo dfu-programmer atmega32u4 erase
	sudo dfu-programmer atmega32u4 flash $^
	sudo dfu-programmer atmega32u4 start 

clean:
	rm -rf $(BUILD_DIR)