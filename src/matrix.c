#include "matrix.h"
#include "utils.h"
#include <stdint.h>
#include <string.h>

pin_t cols[NUM_COLS];
pin_t rows[NUM_ROWS];

void matrix_init(void) {
    pin_init(&cols[0], 0x03, 5, 0);
    pin_init(&cols[1], 0x09, 7, 0);
    pin_init(&cols[2], 0x09, 6, 0);
    pin_init(&cols[3], 0x09, 4, 0);

    pin_init(&rows[0], 0x03, 4, 1);
    pin_init(&rows[1], 0x03, 6, 1);
    pin_init(&rows[2], 0x06, 6, 1);
    pin_init(&rows[3], 0x06, 7, 1);
}

void matrix_get_report(report_t *const report) {
    uint8_t used_key_codes = 0;
    uint8_t layer = 0;
    uint8_t pressed_keys[NUM_ROWS][NUM_COLS];
    memset(pressed_keys, 0, sizeof(pressed_keys));
    memset(report, 0, sizeof(report_t));

    /* The matrix uses inverted logic, which means a key is pressed, when the corresponding pin reads 0. */
    for (int i = 0; i < NUM_ROWS; i++) {
        pin_set_low(&rows[i]);
        for (int j = 0; j < NUM_COLS; j++) {
            if (!pin_read(&cols[j])) {
                if (layer_switch_keys[i][j] > 0) {
                    layer |= layer_switch_keys[i][j];
                }
                pressed_keys[i][j] = 1;
            }
        }
        pin_set_high(&rows[i]);
    }

    /*create hid report*/
    for (int i = 0; i < NUM_ROWS; i++) {
        for (int j = 0; j < NUM_COLS; j++) {
            if (pressed_keys[i][j] && layer_switch_keys[i][j] == 0 && used_key_codes < 6) {
                report->keys[used_key_codes++] = matrix[layer][i][j];
            }
        }
    }
}

void pin_init(pin_t *pin, uint8_t addr, uint8_t offset, uint8_t is_output) {
    pin->pin_reg = &_SFR_IO8(addr);
    pin->data_reg = &_SFR_IO8(addr + 0x01);
    pin->port_reg = &_SFR_IO8(addr + 0x02);
    pin->offset = offset;

    if (is_output) {
        SETBIT(*pin->data_reg, pin->offset);
        pin_set_high(pin);
    } else {
        CLEARBIT(*pin->data_reg, pin->offset);
        SETBIT(*pin->port_reg, pin->offset);
    }
}

void pin_set_high(pin_t *pin) { SETBIT(*pin->port_reg, pin->offset); }

void pin_set_low(pin_t *pin) { CLEARBIT(*pin->port_reg, pin->offset); }

uint8_t pin_read(pin_t *pin) { return (*pin->pin_reg & (1 << pin->offset)); }
