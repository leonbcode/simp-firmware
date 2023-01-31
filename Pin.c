#include "Pin.h"

void Pin_Init(Pin *pin, uint8_t reg_addr, uint8_t offs, uint8_t isOutput) {
  pin->pinReg = &_SFR_IO8(reg_addr);
  pin->dataReg = &_SFR_IO8(reg_addr + 0x01);
  pin->portReg = &_SFR_IO8(reg_addr + 0x02);

  pin->offset = offs;

  if (isOutput) {
    // set data direction and pin to high
    SETBIT(*pin->dataReg, pin->offset);
    Pin_SetHigh(pin);
  } else {
    // set data direction and activate the internal pull-up resistor
    CLEARBIT(*pin->dataReg, pin->offset);
    SETBIT(*pin->portReg, pin->offset);
  }
}

void Pin_SetHigh(Pin *pin) { SETBIT(*pin->portReg, pin->offset); }

void Pin_SetLow(Pin *pin) { CLEARBIT(*pin->portReg, pin->offset); }

uint8_t Pin_Read(Pin *pin) { return (*pin->pinReg & (1 << pin->offset)); }
