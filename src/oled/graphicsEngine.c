#include <avr/pgmspace.h>
#include <stdint.h>
#include <string.h>

#include "oled/graphicsEngine.h"

element_t *layers;
size_t layerCount;

void initGraphicsEngine(element_t *elements, size_t size) {
    layers = elements;
    layerCount = size;
}

void prepareNextFrame(element_t *element) {
    element->frameCounter++;

    element->pos.x += element->vel.x;
    element->pos.y += element->vel.y;

    if (!element->isStatic && element->frameCounter > element->state->duration) {
        element->frameCounter = 0;
        element->state = element->state->next;
    }

    if (element->on_frame_update == NULL) {
        return;
    }
    element->on_frame_update(element);
}

void setPixelFromByte(uint8_t *buffer, int8_t row, int8_t col, uint8_t dataByte, uint8_t isWrapping) {
    // TODO: wrap around behaviour
    if (!dataByte) {
        return;
    }
    if (col < 0) {
        if (!isWrapping) {
            return;
        }
        col += SCREEN_WIDTH;
    }
    uint8_t offset = row % 8;
    uint16_t pos = ((row - offset) / 8 * SCREEN_WIDTH) + col;
    buffer[pos] |= (dataByte << offset);
    if (offset == 0 || pos >= BUFFER_SIZE - SCREEN_WIDTH) {
        return;
    }
    pos += SCREEN_WIDTH;
    buffer[pos] |= (dataByte >> (8 - offset));
}

void renderFrame(uint8_t *frameBuffer) {
    memset(frameBuffer, 0, BUFFER_SIZE);

    for (size_t i = 0; i < layerCount; i++) {
        prepareNextFrame(&layers[i]);
    }

    // loop through each layer
    for (uint8_t l = 0; l < layerCount; l++) {
        // skip if layer isn't visible
        if (!layers[l].isVisible) {
            continue;
        }

        sprite_t *sprite = layers[l].state->sprite;

        for (int16_t x = 0; x < sprite->size; x++) {

            uint8_t row_offs = x % sprite->width;
            uint16_t page_offs = (x - row_offs) / sprite->width * 8;

            setPixelFromByte(frameBuffer, layers[l].pos.y + page_offs, layers[l].pos.x + row_offs,
                             pgm_read_byte(&sprite->bitmap[x]), layers[l].isWrapping);
        }
    }
}