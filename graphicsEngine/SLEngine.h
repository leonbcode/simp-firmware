#ifndef _SLENGINE_H_
#define _SLENGINE_H_

#include "LUFA/Common/Common.h"
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <stdint.h>

#define BUFFER_SIZE 512

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
  uint8_t frame_counter;
  Pair pos, vel;
  uint8_t isVisible;
  uint8_t isStatic;
  uint8_t hasChanged;
  State *state;
} Element;

void initEngine(Element *elements, uint8_t size);
uint8_t renderFrame(uint8_t *frame_buffer);
void setPixelFromByte(uint8_t *buffer, uint8_t row, uint8_t col, uint8_t byte);
void prepareNextFrame(Element *element);

#endif