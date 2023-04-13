#ifndef _MATRIX_H_
#define _MATRIX_H_

#include <avr/io.h>
#include <stdint.h>

#include <LUFA/Drivers/USB/USB.h>

#include "Pin.h"

#define NUM_COLS   4
#define NUM_ROWS   4
#define NUM_LAYERS 3

void MATRIX_Init(void);
void MATRIX_GetReport(USB_KeyboardReport_Data_t* const report_data);
#endif