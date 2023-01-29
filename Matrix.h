#ifndef _MATRIX_H_
#define _MATRIX_H_

#include <avr/io.h>
#include <stdint.h>

#include <LUFA/Drivers/USB/USB.h>

#include "Pin.h"

#define NUM_COLS 4
#define NUM_ROWS 4
#define NUM_LAYERS 1

void Matrix_Init(void);
void Matrix_GetReport(USB_KeyboardReport_Data_t *const ReportData);
#endif