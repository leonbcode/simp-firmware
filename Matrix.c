#include "Matrix.h"

struct Pin cols[NUM_COLS];
struct Pin rows[NUM_ROWS];

const uint8_t matrix[NUM_LAYERS][NUM_ROWS][NUM_COLS] = {
    {{HID_KEYBOARD_SC_A, HID_KEYBOARD_SC_B, HID_KEYBOARD_SC_C, HID_KEYBOARD_SC_D},
     {HID_KEYBOARD_SC_E, HID_KEYBOARD_SC_F, HID_KEYBOARD_SC_G, HID_KEYBOARD_SC_H},
     {HID_KEYBOARD_SC_I, HID_KEYBOARD_SC_J, HID_KEYBOARD_SC_K, HID_KEYBOARD_SC_L},
     {HID_KEYBOARD_SC_M, HID_KEYBOARD_SC_N, HID_KEYBOARD_SC_O, HID_KEYBOARD_SC_P}}};

void Matrix_Init(void) {
  Pin_Init(&cols[0], 0x03, 5, 0);
  Pin_Init(&cols[1], 0x09, 7, 0);
  Pin_Init(&cols[2], 0x09, 6, 0);
  Pin_Init(&cols[3], 0x09, 4, 0);

  Pin_Init(&rows[0], 0x03, 4, 1);
  Pin_Init(&rows[1], 0x03, 6, 1);
  Pin_Init(&rows[2], 0x06, 6, 1);
  Pin_Init(&rows[3], 0x06, 7, 1);
}

void Matrix_GetReport(USB_KeyboardReport_Data_t *const ReportData) {

  uint8_t usedKeyCodes = 0;

  // clear data
  memset(ReportData, 0, sizeof(USB_KeyboardReport_Data_t));

  // The matrix uses inverted logic, which means a key is pressed, when the
  // corresponding pin reads 0.
  // Resulting of this the rows are set to high by default.
  for (int i = 0; i < NUM_ROWS; i++) {
    Pin_SetLow(&rows[i]);
    for (int j = 0; j < NUM_COLS; j++) {
      if (!Pin_Read(&cols[j]) && usedKeyCodes < 6) {
        ReportData->KeyCode[usedKeyCodes++] = matrix[0][i][j];
      }
    }
    Pin_SetHigh(&rows[i]);
  }
}