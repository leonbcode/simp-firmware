#include "Pin.h"

void initPin(struct Pin *pin, uint8_t addr, uint8_t offs, uint8_t isOutput) {
  pin->pinReg = &_SFR_IO8(addr);
  pin->dataReg = &_SFR_IO8(addr + 0x01);
  pin->portReg = &_SFR_IO8(addr + 0x02);
  pin->offset = offs;

  if (isOutput) {
    SETBIT(*pin->dataReg, pin->offset);
  } else {
    CLEARBIT(*pin->dataReg, pin->offset);
    SETBIT(*pin->portReg, pin->offset);
  }
}

void setHigh(struct Pin *pin) { SETBIT(*pin->portReg, pin->offset); }

void setLow(struct Pin *pin) { CLEARBIT(*pin->portReg, pin->offset); }

uint8_t readPin(struct Pin *pin) { return *pin->pinReg & (1 << pin->offset); }
