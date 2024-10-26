#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/power.h>
#include <avr/wdt.h>
#include <string.h>

#include "matrix.h"
#include "oled/bitmaps.h"
#include "oled/graphicsEngine.h"
#include "oled/ssd1306.h"
#include "usb.h"
#include "utils.h"

volatile unsigned long millis;

ISR(TIMER1_COMPA_vect) { millis++; }

void setupHardware(void) {
    /* Disable watchdog if enabled by bootloader/fuses */
    CLEARBIT(MCUSR, WDRF);
    wdt_disable();

    /* setup timer and prescaler (1024) */
    SETBIT(TCCR1B, WGM12);
    SETBIT(TCCR1B, CS11);
    SETBIT(TCCR1B, CS10);
    /* set compare value */
    OCR1A = 250;
    /* enable compare match interrupt */
    SETBIT(TIMSK1, OCIE1A);
    /* enable global interrupts */
    sei();

    /* Disable clock division */
    clock_prescale_set(clock_div_1);

    OLED_Init();
    usb_init();
}

void initializeGraphicsEngine(void) {
    // static sprite_t dino0_sprite = {16, 32, dino0};
    static sprite_t dino1_sprite = {16, 32, dino1};
    static sprite_t dino2_sprite = {16, 32, dino2};
    static sprite_t bird0_sprite = {16, 32, bird0};
    static sprite_t bird1_sprite = {16, 32, bird1};
    static sprite_t cactus0_sprite = {12, 24, cactus0};
    static sprite_t ground_sprite = {128, 128, ground};

    // static state_t dino0_state = {1, &dino0_sprite, NULL};
    static state_t dino1_state = {1, &dino1_sprite, NULL};
    static state_t dino2_state = {1, &dino2_sprite, &dino1_state};
    dino1_state.next = &dino2_state;

    static state_t bird0_state = {4, &bird0_sprite, NULL};
    static state_t bird1_state = {4, &bird1_sprite, &bird0_state};
    bird0_state.next = &bird1_state;
    static state_t cauctus0_state = {0, &cactus0_sprite, NULL};
    static state_t ground_state = {0, &ground_sprite, NULL};

    static element_t elements[] = {{.frameCounter = 0,
                                    .pos = {.x = 0, .y = 30},
                                    .vel = {.x = -2, .y = 0},
                                    .isVisible = 1,
                                    .isStatic = 1,
                                    .isWrapping = 1,
                                    .state = &ground_state,
                                    .on_frame_update = NULL},
                                   {.frameCounter = 0,
                                    .pos = {.x = 64, .y = 16},
                                    .vel = {.x = -2, .y = 0},
                                    .isVisible = 1,
                                    .isStatic = 1,
                                    .isWrapping = 0,
                                    .state = &cauctus0_state,
                                    .on_frame_update = NULL},
                                   {.frameCounter = 0,
                                    .pos = {.x = -64, .y = 15},
                                    .vel = {.x = -2, .y = 0},
                                    .isVisible = 1,
                                    .isStatic = 0,
                                    .isWrapping = 0,
                                    .state = &bird0_state,
                                    .on_frame_update = NULL},
                                   {.frameCounter = 0,
                                    .pos = {.x = 8, .y = 14},
                                    .vel = {.x = 0, .y = 0},
                                    .isVisible = 1,
                                    .isStatic = 0,
                                    .isWrapping = 0,
                                    .state = &dino1_state,
                                    .on_frame_update = NULL}};

    initGraphicsEngine(elements, sizeof(elements) / sizeof(element_t));
}

void oledTask(void) {
    static unsigned long startTime;
    static uint8_t prevBuffer[BUFFER_SIZE];
    uint8_t buffer[BUFFER_SIZE];

    if (millis - startTime >= 40) {
        startTime = millis;
    }

    renderFrame(buffer);
    if (memcmp(prevBuffer, buffer, BUFFER_SIZE) == 0) {
        return;
    }
    memcpy(prevBuffer, buffer, BUFFER_SIZE);
    OLED_DisplayFrame(buffer);
}

void sendTestReport(void) {
    static report_t report = {0, {0x04, 0, 0, 0, 0, 0}};
    usb_send(report);
    report.keys[0] = 0;
    usb_send(report);
}

void hidTask(void) {
        sendTestReport();
}

int main(void) {
    setupHardware();
    initializeGraphicsEngine();

    while (!get_usb_config_status())
        ;

    while (1) {
        hidTask();
        oledTask();
    }
}