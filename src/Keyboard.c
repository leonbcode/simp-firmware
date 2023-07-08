#include "Keyboard.h"

volatile unsigned long millis;
static uint8_t prev_buffer[BUFFER_SIZE];
static uint8_t buffer[BUFFER_SIZE];

ISR(TIMER1_COMPA_vect) { millis++; }

int main(void) {

    setup_hardware();
    GlobalInterruptEnable();
    init_graphics_engine();

    while (1) {
        HID_Task();
        USB_USBTask();
        OLED_Task();
    }
}

void setup_hardware(void) {
    /* Disable watchdog if enabled by bootloader/fuses */
    MCUSR &= ~(1 << WDRF);
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

    /* Hardware Initialization */
    MATRIX_Init();
    USB_Init();
    OLED_Init();
}

void test(element_t* element) {}

void init_graphics_engine(void) {
    memset(prev_buffer, 0, BUFFER_SIZE);
    memset(buffer, 0, BUFFER_SIZE);

    /*     static sprite_t moon0_sprite = {20, moon0, 60};
    static sprite_t moon1_sprite = {20, moon1, 60};
    static sprite_t moon2_sprite = {20, moon2, 60};
    static sprite_t moon3_sprite = {20, moon3, 60};
    static sprite_t moon4_sprite = {20, moon4, 60};
    static sprite_t moon5_sprite = {20, moon5, 60};
    static sprite_t moon6_sprite = {20, moon6, 60};
    static sprite_t moon7_sprite = {20, moon7, 60};
    static sprite_t sky_sprite = {128, sky, 512};

    static state_t moon7_state = {3, &moon7_sprite, NULL};
    static state_t moon6_state = {3, &moon6_sprite, &moon7_state};
    static state_t moon5_state = {3, &moon5_sprite, &moon6_state};
    static state_t moon4_state = {3, &moon4_sprite, &moon5_state};
    static state_t moon3_state = {3, &moon3_sprite, &moon4_state};
    static state_t moon2_state = {3, &moon2_sprite, &moon3_state};
    static state_t moon1_state = {3, &moon1_sprite, &moon2_state};
    static state_t moon0_state = {3, &moon0_sprite, &moon1_state};
    moon7_state.next = &moon0_state;
    static state_t sky_state = {0, &sky_sprite, NULL}; */

    static sprite_t dino0_sprite = {16, dino0, 32};
    static sprite_t dino1_sprite = {16, dino1, 32};
    static sprite_t dino2_sprite = {16, dino2, 32};
    static sprite_t bird0_sprite = {16, bird0, 32};
    static sprite_t bird1_sprite = {16, bird1, 32};
    static sprite_t cactus0_sprite = {12, cactus0, 24};
    static sprite_t ground_sprite = {128, ground, 128};

    static state_t dino0_state = {0, &dino0_sprite, NULL};
    static state_t dino1_state = {0, &dino1_sprite, NULL};
    static state_t dino2_state = {0, &dino2_sprite, &dino1_state};
    dino1_state.next = &dino2_state;

    static state_t bird0_state = {2, &bird0_sprite, NULL};
    static state_t bird1_state = {2, &bird1_sprite, &bird0_state};
    bird0_state.next = &bird1_state;
    static state_t cauctus0_state = {0, &cactus0_sprite, NULL};
    static state_t ground_state = {0, &ground_sprite, NULL};

    /* {0, {98, 6}, {0, 0}, 1, 0, 1, &moon0_state}, {0, {0, 0}, {0, 0}, 1, 1, 1, &sky_state, NULL} */
    static element_t elements[] = {{.frame_counter = 0,
                                    .pos = {.x = 0, .y = 30},
                                    .vel = {.x = 0, .y = 0},
                                    .is_visible = 1,
                                    .is_static = 1,
                                    .has_changed = 1,
                                    .state = &ground_state,
                                    .on_frame_update = NULL},
                                   {.frame_counter = 0,
                                    .pos = {.x = 8, .y = 14},
                                    .vel = {.x = 0, .y = 0},
                                    .is_visible = 1,
                                    .is_static = 0,
                                    .has_changed = 1,
                                    .state = &dino1_state,
                                    .on_frame_update = test},
                                   {.frame_counter = 0,
                                    .pos = {.x = 100, .y = 15},
                                    .vel = {.x = 0, .y = 0},
                                    .is_visible = 1,
                                    .is_static = 0,
                                    .has_changed = 1,
                                    .state = &bird0_state,
                                    .on_frame_update = test},
                                   {.frame_counter = 0,
                                    .pos = {.x = 64, .y = 16},
                                    .vel = {.x = 0, .y = 0},
                                    .is_visible = 1,
                                    .is_static = 1,
                                    .has_changed = 1,
                                    .state = &cauctus0_state,
                                    .on_frame_update = NULL}};

    SLE_InitEngine(elements, sizeof(elements) / sizeof(element_t));
}

/** Function to manage HID report generation and transmission to the host, when
 * in report mode. */
void HID_Task(void) {
    /* Device must be connected and configured for the task to run */
    if (USB_DeviceState != DEVICE_STATE_Configured) {
        return;
    }

    send_next_report();
}

void OLED_Task(void) {
    static unsigned long startTime;

    if (millis - startTime >= 100) {
        startTime = millis;

        if (!SLE_RenderFrame(buffer)) {
            return;
        }
        if (memcmp(prev_buffer, buffer, BUFFER_SIZE) == 0) {
            return;
        }

        memcpy(prev_buffer, buffer, BUFFER_SIZE);
        OLED_DisplayFrame(prev_buffer);
    }
}

/** Event handler for the USB_ConfigurationChanged event. This is fired when the
 * host sets the current configuration of the USB device after enumeration, and
 * configures the keyboard device endpoints.
 */
void EVENT_USB_Device_ConfigurationChanged(void) {
    /* Setup HID Report Endpoints */
    Endpoint_ConfigureEndpoint(KEYBOARD_IN_EPADDR, EP_TYPE_INTERRUPT, KEYBOARD_EPSIZE, 1);
    Endpoint_ConfigureEndpoint(KEYBOARD_OUT_EPADDR, EP_TYPE_INTERRUPT, KEYBOARD_EPSIZE, 1);
}

/** Sends the next HID report to the host, via the keyboard data endpoint. */
void send_next_report(void) {
    static USB_KeyboardReport_Data_t PrevKeyboardReportData;
    USB_KeyboardReport_Data_t KeyboardReportData;

    /* Create the next keyboard report for transmission to the host */
    MATRIX_GetReport(&KeyboardReportData);

    /* Check to see if the report data has changed - if so a report MUST be sent
   */
    if (memcmp(&PrevKeyboardReportData, &KeyboardReportData, sizeof(USB_KeyboardReport_Data_t)) == 0) {
        return;
    }

    /* Select the Keyboard Report Endpoint */
    Endpoint_SelectEndpoint(KEYBOARD_IN_EPADDR);

    /* Check if Keyboard Endpoint Ready for Read/Write */
    if (!Endpoint_IsReadWriteAllowed()) {
        return;
    }

    /* Save the current report data for later comparison to check for changes */
    PrevKeyboardReportData = KeyboardReportData;

    /* Write Keyboard Report Data */
    Endpoint_Write_Stream_LE(&KeyboardReportData, sizeof(KeyboardReportData), NULL);

    /* Finalize the stream transfer to send the last packet */
    Endpoint_ClearIN();
}