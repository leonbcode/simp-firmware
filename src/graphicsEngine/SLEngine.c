#include "SLEngine.h"

element_t* layers;
uint8_t layer_count;

void set_pixel_from_byte(uint8_t* buffer, int8_t row, int8_t col, uint8_t byte);
void prepare_next_frame(element_t* element);

void SLE_InitEngine(element_t* elements, const size_t size) {
    layers = elements;
    layer_count = size;
}

uint8_t SLE_RenderFrame(uint8_t* frame_buffer) {
    memset(frame_buffer, 0, BUFFER_SIZE);

    uint8_t render_new_frame = 0;
    for (uint8_t l = 0; l < layer_count; l++) {
        if (layers[l].is_static && !layers[l].has_changed) {
            continue;
        }
        prepare_next_frame(&layers[l]);
        render_new_frame = render_new_frame | layers[l].has_changed;
    }

    if (!render_new_frame) {
        return 0;
    }

    // loop through each layer
    for (uint8_t l = 0; l < layer_count; l++) {
        // skip if layer isn't visible
        if (!layers[l].is_visible) {
            continue;
        }

        sprite_t* sprite = layers[l].state->sprite;

        for (uint16_t x = 0; x < sprite->size; x++) {

            uint8_t row_offs = x % sprite->width;
            uint16_t page_offs = (x - row_offs) / sprite->width * 8;

            set_pixel_from_byte(frame_buffer, layers[l].pos.y + page_offs, layers[l].pos.x + row_offs,
                                pgm_read_byte(&sprite->bitmap[x]));
        }

        layers[l].has_changed = 0;
    }
    return 1;
}

void set_pixel_from_byte(uint8_t* buffer, int8_t row, int8_t col, uint8_t data_byte) {
    //TODO: wrap around behaviour
    if (!data_byte) {
        return;
    }
    if(col < 0){
        col += SCREEN_WIDTH;
    }
    uint8_t offset = row % 8;
    uint16_t pos = ((row - offset) / 8 * SCREEN_WIDTH) + col;
    buffer[pos] |= (data_byte << offset);
    if (offset == 0 || pos >= BUFFER_SIZE - SCREEN_WIDTH) {
        return;
    }
    pos += SCREEN_WIDTH;
    buffer[pos] |= (data_byte >> (8 - offset));
}

void prepare_next_frame(element_t* element) {
    element->frame_counter++;

    element->pos.x += element->vel.x;
    element->pos.y += element->vel.y;

    if (element->frame_counter > element->state->duration && !element->is_static) {
        // jump to next state
        element->frame_counter = 0;
        element->has_changed = 1;
        element->state = element->state->next;
    }
    if (element->on_frame_update == NULL) {
        return;
    }
    element->on_frame_update(element);
}