#ifndef _MATRIX_H_
#define _MATRIX_H_

#include <avr/io.h>
#include <stdbool.h>
#include <stdint.h>

#include <LUFA/Drivers/USB/USB.h>

#include "Pin.h"

#define NUM_COLS 4
#define NUM_ROWS 4

void initMatrix(void);
void getReport(USB_KeyboardReport_Data_t *const ReportData);
#endif