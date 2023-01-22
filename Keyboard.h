#ifndef _KEYBOARD_H_
#define _KEYBOARD_H_

/* Includes: */
#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/power.h>
#include <avr/wdt.h>
#include <stdbool.h>
#include <string.h>

#include <LUFA/Drivers/USB/USB.h>
#include <LUFA/Platform/Platform.h>

#include "Descriptors.h"
#include "Matrix.h"
#include "Pin.h"

/* Function Prototypes: */
void SetupHardware(void);
void HID_Task(void);

void EVENT_USB_Device_Connect(void);
void EVENT_USB_Device_Disconnect(void);
void EVENT_USB_Device_ConfigurationChanged(void);
void EVENT_USB_Device_StartOfFrame(void);

void CreateKeyboardReport(USB_KeyboardReport_Data_t *const ReportData);
void SendNextReport(void);

#endif
