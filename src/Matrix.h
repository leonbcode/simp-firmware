#ifndef _MATRIX_H_
#define _MATRIX_H_

#include <avr/io.h>
#include <stdint.h>

#include <LUFA/Drivers/USB/USB.h>

#include "Pin.h"

#define NUM_COLS   4
#define NUM_ROWS   4
#define NUM_LAYERS 1

static const uint8_t matrix[NUM_LAYERS][NUM_ROWS][NUM_COLS] = {
    {{HID_KEYBOARD_SC_F13, HID_KEYBOARD_SC_F14, HID_KEYBOARD_SC_F15, HID_KEYBOARD_SC_F16},
     {HID_KEYBOARD_SC_F17, HID_KEYBOARD_SC_F18, HID_KEYBOARD_SC_F19, HID_KEYBOARD_SC_F20},
     {HID_KEYBOARD_SC_F21, HID_KEYBOARD_SC_F22, HID_KEYBOARD_SC_F23, HID_KEYBOARD_SC_F24},
     {HID_KEYBOARD_SC_KEYPAD_BINARY, HID_KEYBOARD_SC_KEYPAD_OCTAL, HID_KEYBOARD_SC_KEYPAD_DECIMAL,
      HID_KEYBOARD_SC_KEYPAD_HEXADECIMAL}}};

static const uint8_t layer_switch_keys[NUM_ROWS][NUM_COLS] = {{0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}};

void MATRIX_Init(void);
void MATRIX_GetReport(USB_KeyboardReport_Data_t* const report_data);
#endif