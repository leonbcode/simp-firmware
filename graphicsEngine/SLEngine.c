#include "SLEngine.h"

Element *layers;
uint8_t layer_count;

void initEngine(Element *elements, uint8_t size) {
  layers = elements;
  layer_count = size;
}

uint8_t renderFrame(uint8_t *frame_buffer) {
  memset(frame_buffer, 0, BUFFER_SIZE);

  uint8_t renderNewFrame = 0;
  for (uint8_t l = 0; l < layer_count; l++) {
    renderNewFrame = renderNewFrame | layers[l].hasChanged;
    if (layers[l].isStatic)
      continue;
    prepareNextFrame(&layers[l]);
  }

  if (!renderNewFrame)
    return 0;

  // loop thorugh each layer
  for (uint8_t l = 0; l < layer_count; l++) {
    // skip if layer isn't visible
    if (!layers[l].isVisible)
      continue;

    Sprite *sprite = layers[l].state->sprite;

    for (uint16_t x = 0; x < sprite->size; x++) {

      uint8_t row_offs = x % sprite->width;
      uint16_t page_offs = (x - row_offs) / sprite->width * 8;

      setPixelFromByte(frame_buffer, layers[l].pos.y + page_offs, layers[l].pos.x + row_offs,
                       pgm_read_byte(&sprite->bitmap[x]));
    }

    layers[l].hasChanged = 0;
  }
  return 1;
}

void setPixelFromByte(uint8_t *buffer, uint8_t row, uint8_t col, uint8_t data_byte) {
  if (!data_byte)
    return;
  uint8_t offset = row % 8;
  uint16_t pos = ((row - offset) / 8 * 128) + col;
  buffer[pos] |= (data_byte << offset);
  if (offset == 0 || pos >= BUFFER_SIZE - 128)
    return;
  pos += 128;
  buffer[pos] |= (data_byte >> (8 - offset));
}

void prepareNextFrame(Element *element) {
  element->frame_counter++;

  element->pos.x += element->vel.x;
  element->pos.y += element->vel.y;

  if (element->frame_counter > element->state->duration) {
    // jump to next state
    element->frame_counter = 0;
    element->hasChanged = 1;
    element->state = layers->state->next;
  }
}