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
  // TODO: render optimization
  // clear buffer
  memset(frame_buffer, 0, 512);

  // loop thorugh each layer
  for (uint8_t l = 0; l < layer_count; l++) {
    // skip if layer isn't visible
    if (!layers[l].isVisible)
      continue;

    Sprite *sprite = layers[l].state->sprite;
    for (uint16_t x = 0; x < sprite->size; x++) {

      uint8_t x_offs = x % sprite->width;
      uint16_t y_offs = (x - x_offs) / sprite->width * 8;

      setPixelFromByte(frame_buffer, layers[l].pos.y + y_offs, layers[l].pos.x + x_offs,
                       pgm_read_byte(&sprite->bitmap[x]));
    }

    if (layers[l].isStatic)
      continue;
    nextFrame(&layers[l]);
  }
}

void setPixelFromByte(uint8_t *buffer, uint8_t row, uint8_t col, uint8_t byte) {
  if (!byte)
    return;
  uint8_t offset = row % 8;
  uint16_t pos = ((row - offset) / 8 * 128) + col;
  buffer[pos] |= (byte << offset);
  if (offset == 0)
    return;
  pos += 128;
  if (pos >= 512)
    return;
  buffer[pos] |= (byte >> (8 - offset));
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
