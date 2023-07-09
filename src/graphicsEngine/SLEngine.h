#ifndef _SLENGINE_H_
#define _SLENGINE_H_

#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <stdint.h>

#include "LUFA/Common/Common.h"

#define BUFFER_SIZE 512

typedef struct {
    uint8_t x, y;
} pair_t;

typedef struct {
    uint8_t width;
    const uint8_t* bitmap;
    uint16_t size;
} sprite_t;

typedef struct State state_t;
struct State {
    uint8_t duration;
    sprite_t* sprite;
    struct State* next;
};

typedef struct Element element_t;
struct Element{
    uint8_t frame_counter;
    pair_t pos, vel;
    uint8_t is_visible;
    uint8_t is_static;
    uint8_t has_changed;
    state_t* state;

    void (*on_frame_update)(element_t* element);
};

void SLE_InitEngine(element_t* elements, const size_t size);
uint8_t SLE_RenderFrame(uint8_t* frame_buffer);

#endif