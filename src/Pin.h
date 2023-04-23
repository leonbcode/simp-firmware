#ifndef _PIN_H_
#define _PIN_H_

#include <avr/sfr_defs.h>
#include <stdint.h>

#define SETBIT(reg, offs)   (reg |= (1 << offs))
#define CLEARBIT(reg, offs) (reg &= ~(1 << offs))

typedef struct {
    volatile uint8_t* data_reg;
    volatile uint8_t* port_reg;
    volatile uint8_t* pin_reg;
    uint8_t offset;
} Pin;

void PIN_Init(Pin* pin, uint8_t addr, uint8_t offset, uint8_t is_output);
void PIN_SetHigh(Pin* pin);
void PIN_SetLow(Pin* pin);
uint8_t PIN_Read(Pin* pin);

#endif