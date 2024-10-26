#pragma once

#include <stddef.h>
#include <stdint.h>

#define BUFFER_SIZE 512
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32

typedef struct {
    uint8_t x, y;
} pair_t;

typedef struct {
    int8_t width;
    int16_t size;
    const uint8_t *bitmap;
} sprite_t;

typedef struct state state_t;
struct state {
    int8_t duration;
    sprite_t *sprite;
    struct state *next;
};

typedef struct element element_t;
struct element {
    int8_t frameCounter;
    pair_t pos, vel;
    uint8_t isVisible, isStatic, isWrapping;
    state_t *state;

    void (*on_frame_update)(element_t *element);
};

void initGraphicsEngine(element_t *elements, size_t size);
void renderFrame(uint8_t *frameBuffer);