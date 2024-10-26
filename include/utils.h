#pragma once

#define SETBIT(reg, offs) (reg |= (1 << offs))
#define CLEARBIT(reg, offs) (reg &= ~(1 << offs))