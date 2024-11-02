#pragma once

#include <avr/sfr_defs.h>
#include <stdint.h>

typedef struct report_t {
    uint8_t modifiers;
    uint8_t keys[6];
} report_t;

typedef struct {
    volatile uint8_t *data_reg;
    volatile uint8_t *port_reg;
    volatile uint8_t *pin_reg;
    uint8_t offset;
} pin_t;

void matrix_init(void);
void matrix_get_report(report_t *const report);

void pin_init(pin_t *pin, uint8_t addr, uint8_t offset, uint8_t is_output);
void pin_set_high(pin_t *pin);
void pin_set_low(pin_t *pin);
uint8_t pin_read(pin_t *pin);

#define NUM_COLS   4
#define NUM_ROWS   4
#define NUM_LAYERS 1

static const uint8_t matrix[NUM_LAYERS][NUM_ROWS][NUM_COLS] = {
    {{0x04, 0x05, 0x06, 0x07},
     {0x04, 0x05, 0x06, 0x07},
     {0x04, 0x05, 0x06, 0x07},
     {0x04, 0x05, 0x06, 0x07}}};