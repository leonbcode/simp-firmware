#include "Matrix.h"
#include <util/delay.h>

struct Pin cols[NUM_COLS];
struct Pin rows[NUM_ROWS];

const uint8_t matrix[NUM_COLS][NUM_ROWS] = {
    {HID_KEYBOARD_SC_A, HID_KEYBOARD_SC_B, HID_KEYBOARD_SC_C,
     HID_KEYBOARD_SC_D},
    {HID_KEYBOARD_SC_E, HID_KEYBOARD_SC_F, HID_KEYBOARD_SC_G,
     HID_KEYBOARD_SC_H},
    {HID_KEYBOARD_SC_I, HID_KEYBOARD_SC_J, HID_KEYBOARD_SC_K,
     HID_KEYBOARD_SC_L},
    {HID_KEYBOARD_SC_M, HID_KEYBOARD_SC_N, HID_KEYBOARD_SC_O,
     HID_KEYBOARD_SC_P}};

void initMatrix(void) {
  initPin(&cols[0], 0x03, 5, 1);
  initPin(&cols[1], 0x09, 7, 1);
  initPin(&cols[2], 0x09, 6, 1);
  initPin(&cols[3], 0x09, 4, 1);

  initPin(&rows[0], 0x03, 4, 0);
  initPin(&rows[1], 0x03, 6, 0);
  initPin(&rows[2], 0x06, 6, 0);
  initPin(&rows[3], 0x06, 7, 0);
}

void getReport(USB_KeyboardReport_Data_t *const ReportData) {

  uint8_t usedKeyCodes = 0;

  // clear data
  memset(ReportData, 0, sizeof(USB_KeyboardReport_Data_t));

  for (int i = 0; i < NUM_COLS; i++) {
    setHigh(&cols[i]);
    _delay_ms(250);
    for (int j = 0; j < NUM_ROWS; j++) {
      if (readPin(&rows[j]) == 0 && usedKeyCodes < 6) {
        ReportData->KeyCode[usedKeyCodes++] = matrix[i][j];
      }
      _delay_ms(250);
    }
    setLow(&cols[i]);
  }
}