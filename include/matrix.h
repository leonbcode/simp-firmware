#pragma once

#include <stdint.h>

typedef struct report_t {
    uint8_t modifiers;
    uint8_t keys[6];
} report_t;