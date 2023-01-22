#ifndef _PIN_H_
#define _PIN_H_

#include <avr/sfr_defs.h>
#include <stdint.h>

#define SETBIT(reg, offs) (reg |= (1 << offs))
#define CLEARBIT(reg, offs) (reg &= ~(1 << offs))

struct Pin {
  volatile uint8_t *dataReg;
  volatile uint8_t *portReg;
  volatile uint8_t *pinReg;
  uint8_t offset;
};

void initPin(struct Pin *pin, uint8_t addr, uint8_t offs, uint8_t isOutput);
void setHigh(struct Pin *pin);
void setLow(struct Pin *pin);
uint8_t readPin(struct Pin *pin);

#endif