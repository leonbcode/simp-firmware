#include "Pin.h"

void PIN_Init(Pin* pin, uint8_t reg_addr, uint8_t offs, uint8_t is_output) {
    pin->pin_reg = &_SFR_IO8(reg_addr);
    pin->data_reg = &_SFR_IO8(reg_addr + 0x01);
    pin->port_reg = &_SFR_IO8(reg_addr + 0x02);
    pin->offset = offs;

    if (is_output) {
        SETBIT(*pin->data_reg, pin->offset);
        PIN_SetHigh(pin);
    } else {
        CLEARBIT(*pin->data_reg, pin->offset);
        SETBIT(*pin->port_reg, pin->offset);
    }
}

void PIN_SetHigh(Pin* pin) { SETBIT(*pin->port_reg, pin->offset); }

void PIN_SetLow(Pin* pin) { CLEARBIT(*pin->port_reg, pin->offset); }

uint8_t PIN_Read(Pin* pin) { return (*pin->pin_reg & (1 << pin->offset)); }
