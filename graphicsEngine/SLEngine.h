#ifndef _SLENGINE_H_
#define _SLENGINE_H_

#include <stdint.h>

#define PIXEL_ON 1
#define PIXEL_OFF 0

typedef struct Pair {
  uint8_t x, y;
} Pair;

typedef struct Sprite {
  uint8_t width;
  uint8_t *bitmap;
  uint16_t size;
} Sprite;

typedef struct State {
  uint8_t duration;
  Sprite *sprite;
  struct State *next;
} State;

typedef struct Element {
  uint8_t frame;
  Pair pos, vel;
  uint8_t isVisible;
  uint8_t isStatic;
  State *state;
} Element;

void initEngine(Element *elements, uint8_t size);
void renderFrame(uint8_t *frame_buffer);
void setPixel(uint8_t *buffer, uint8_t row, uint8_t col, uint8_t pixelState);

#endif