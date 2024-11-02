#pragma once

#include "matrix.h"
#include <stdint.h>

#define GET_STATUS 0x00
#define CLEAR_FEATURE 0x01
#define SET_FEATURE 0x03
#define SET_ADDRESS 0x05
#define GET_DESCRIPTOR 0x06
#define GET_CONFIGURATION 0x08
#define SET_CONFIGURATION 0x09
#define GET_INTERFACE 0x0A
#define SET_INTERFACE 0x0B

#define idVendor 0x03eb  // Atmel Corp.
#define idProduct 0x2042 // ATMega32u4 DFU Bootloader (This isn't a real product so I don't have legitimate IDs)

#define KEYBOARD_ENDPOINT_NUM 1 // The second endpoint is the HID endpoint

#define CONFIG_SIZE 34
#define HID_OFFSET 18

// HID Class-specific request codes - refer to HID Class Specification
// Chapter 7.2 - Remarks

#define GET_REPORT 0x01
#define GET_IDLE 0x02
#define GET_PROTOCOL 0x03
#define SET_REPORT 0x09
#define SET_IDLE 0x0A
#define SET_PROTOCOL 0x0B

uint8_t get_usb_config_status();
void usb_init(void);
int usb_send(report_t report);