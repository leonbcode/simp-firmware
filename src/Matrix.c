#include "Matrix.h"

Pin cols[NUM_COLS];
Pin rows[NUM_ROWS];

void MATRIX_Init(void) {
    PIN_Init(&cols[0], 0x03, 5, 0);
    PIN_Init(&cols[1], 0x09, 7, 0);
    PIN_Init(&cols[2], 0x09, 6, 0);
    PIN_Init(&cols[3], 0x09, 4, 0);

    PIN_Init(&rows[0], 0x03, 4, 1);
    PIN_Init(&rows[1], 0x03, 6, 1);
    PIN_Init(&rows[2], 0x06, 6, 1);
    PIN_Init(&rows[3], 0x06, 7, 1);
}

void MATRIX_GetReport(USB_KeyboardReport_Data_t* const report_data) {
    uint8_t used_key_codes = 0;
    uint8_t layer = 0;
    uint8_t pressed_keys[NUM_ROWS][NUM_COLS];
    memset(pressed_keys, 0, sizeof(pressed_keys));
    memset(report_data, 0, sizeof(USB_KeyboardReport_Data_t));

    /* The matrix uses inverted logic, which means a key is pressed, when the corresponding pin reads 0. */
    for (int i = 0; i < NUM_ROWS; i++) {
        PIN_SetLow(&rows[i]);
        for (int j = 0; j < NUM_COLS; j++) {
            if (!PIN_Read(&cols[j])) {
                if (layer_switch_keys[i][j] > 0) {
                    layer += layer_switch_keys[i][j];
                }
                pressed_keys[i][j] = 1;
            }
        }
        PIN_SetHigh(&rows[i]);
    }

    /*create hid report*/
    for (int i = 0; i < NUM_ROWS; i++) {
        for (int j = 0; j < NUM_COLS; j++) {
            if (pressed_keys[i][j] && layer_switch_keys[i][j] == 0 && used_key_codes < 6) {
                report_data->KeyCode[used_key_codes++] = matrix[layer][i][j];
            }
        }
    }
}