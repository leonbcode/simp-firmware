MCU          = atmega32u4
ARCH         = AVR8
BOARD        = USBKEY
F_CPU        = 16000000
F_USB        = $(F_CPU)
OPTIMIZATION = s
TARGET       = Keyboard
C_SOURCES    = $(wildcard src/*.c) $(wildcard src/**/*.c)
SRC          = $(C_SOURCES) $(LUFA_SRC_USB) $(LUFA_SRC_USBCLASS) $(LIB)/i2c/i2c.c $(LIB)/oled-ssd1306/SSD1306.c
LUFA_PATH    = libs/lufa/LUFA
LIB 		 = libs
CC_FLAGS     = -DUSE_LUFA_CONFIG_HEADER -Iconfig/

# Default target
all:
# Include LUFA build script makefiles
include $(LUFA_PATH)/Build/lufa_core.mk
include $(LUFA_PATH)/Build/lufa_sources.mk
include $(LUFA_PATH)/Build/lufa_build.mk
include $(LUFA_PATH)/Build/lufa_cppcheck.mk
include $(LUFA_PATH)/Build/lufa_doxygen.mk
include $(LUFA_PATH)/Build/lufa_dfu.mk
include $(LUFA_PATH)/Build/lufa_hid.mk
include $(LUFA_PATH)/Build/lufa_avrdude.mk
include $(LUFA_PATH)/Build/lufa_atprogram.mk


flash:
	$(MAKE) all
	sudo dfu-programmer atmega32u4 erase
	sudo dfu-programmer atmega32u4 flash Keyboard.hex
	sudo dfu-programmer atmega32u4 start