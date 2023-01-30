#include "SLEngine.h"
#include <avr/pgmspace.h>
#include <stdint.h>
#include <string.h>

struct Element *layers;
uint8_t layer_count;

uint8_t prev_frame_buffer[512];

void initEngine(Element *elements, uint8_t size) {
  layers = elements;
  layer_count = size;
}

void renderFrame(uint8_t *frame_buffer) {
  // clear buffer
  memset(frame_buffer, 0, 512);

  // loop thorugh each layer
  for (uint8_t l = 0; l < layer_count; l++) {
    // skip if layer isn't visible
    if (!layers[l].isVisible)
      continue;

    Sprite *sprite = layers[l].state->sprite;
    for (uint16_t x = 0; x < sprite->size; x++) {

      uint16_t offset = (x - (x % sprite->width)) / sprite->width * 8;

      for (uint8_t y = 0; y < 8; y++) {

        setPixel(frame_buffer, layers[l].pos.y + offset + y, layers[l].pos.x + (x % sprite->width),
                 pgm_read_byte(&sprite->bitmap[x]) & (1 << y));
      }
    }

    if (layers[l].isStatic)
      continue;
    nextFrame(&layers[l]);
  }
}

void setPixel(uint8_t *buffer, uint8_t row, uint8_t col, uint8_t pixelState) {
  if (!pixelState)
    return;

  uint8_t offset = row % 8;
  uint16_t pos = ((row - offset) / 8 * 128) + col;
  buffer[pos] |= (1 << offset);
}

void nextFrame(Element *element) {
  element->frame_counter++;

  element->pos.x += element->vel.x;
  element->pos.y += element->vel.y;

  if (element->frame_counter > element->state->duration) {
    // jump to next state
    element->frame_counter = 0;
    element->state = layers->state->next;
  }
}
