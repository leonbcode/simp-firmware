#include "Keyboard.h"

volatile unsigned long millis;
static uint8_t prev_buffer[512];
static uint8_t buffer[512];

ISR(TIMER1_COMPA_vect) { millis++; }

int main(void) {

  SetupHardware();
  GlobalInterruptEnable();
  InitGraphicsEngine();

  while (1) {
    HID_Task();
    USB_USBTask();
    OLED_Task();
  }
}

void SetupHardware(void) {
  /* Disable watchdog if enabled by bootloader/fuses */
  MCUSR &= ~(1 << WDRF);
  wdt_disable();

  // setup timer and prescaler (1024)
  SETBIT(TCCR1B, WGM12);
  SETBIT(TCCR1B, CS11);
  SETBIT(TCCR1B, CS10);
  // set compare value
  OCR1A = 250;
  // enable compare match interrupt
  SETBIT(TIMSK1, OCIE1A);
  // enable global interrupts
  sei();

  /* Disable clock division */
  clock_prescale_set(clock_div_1);

  /* Hardware Initialization */
  Matrix_Init();
  USB_Init();
  OLED_Init();
}

void InitGraphicsEngine(void) {
  memset(prev_buffer, 0, 512);
  memset(buffer, 0, 512);

  static Sprite moon0_sprite = {20, moon0, 60};
  static Sprite moon1_sprite = {20, moon1, 60};
  static Sprite moon2_sprite = {20, moon2, 60};
  static Sprite moon3_sprite = {20, moon3, 60};
  static Sprite moon4_sprite = {20, moon4, 60};
  static Sprite moon5_sprite = {20, moon5, 60};
  static Sprite moon6_sprite = {20, moon6, 60};
  static Sprite moon7_sprite = {20, moon7, 60};

  static State moon7_state = {3, &moon7_sprite, NULL};
  static State moon6_state = {3, &moon6_sprite, &moon7_state};
  static State moon5_state = {3, &moon5_sprite, &moon6_state};
  static State moon4_state = {3, &moon4_sprite, &moon5_state};
  static State moon3_state = {3, &moon3_sprite, &moon4_state};
  static State moon2_state = {3, &moon2_sprite, &moon3_state};
  static State moon1_state = {3, &moon1_sprite, &moon2_state};
  static State moon0_state = {3, &moon0_sprite, &moon1_state};
  moon7_state.next = &moon0_state;

  static Sprite sky_sprite = {128, sky, 512};
  static State sky_state = {0, &sky_sprite, NULL};

  static Element elements[2] = {{0, {98, 6}, {0, 0}, 1, 0, &moon0_state}, {0, {0, 0}, {0, 0}, 1, 1, &sky_state}};
  initEngine(elements, 2);
}

/** Function to manage HID report generation and transmission to the host, when
 * in report mode. */
void HID_Task(void) {
  /* Device must be connected and configured for the task to run */
  if (USB_DeviceState != DEVICE_STATE_Configured)
    return;

  SendNextReport();
}

void OLED_Task(void) {
  static unsigned long startTime;

  if (millis - startTime >= 128) {
    startTime = millis;

    renderFrame(buffer);
    if (memcmp(prev_buffer, buffer, 512) == 0)
      return;

    memcpy(prev_buffer, buffer, 512);
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
void SendNextReport(void) {
  static USB_KeyboardReport_Data_t PrevKeyboardReportData;
  USB_KeyboardReport_Data_t KeyboardReportData;

  /* Create the next keyboard report for transmission to the host */
  Matrix_GetReport(&KeyboardReportData);

  /* Check to see if the report data has changed - if so a report MUST be sent
   */
  if (memcmp(&PrevKeyboardReportData, &KeyboardReportData, sizeof(USB_KeyboardReport_Data_t)) == 0)
    return;

  /* Select the Keyboard Report Endpoint */
  Endpoint_SelectEndpoint(KEYBOARD_IN_EPADDR);

  /* Check if Keyboard Endpoint Ready for Read/Write */
  if (!Endpoint_IsReadWriteAllowed())
    return;

  /* Save the current report data for later comparison to check for changes */
  PrevKeyboardReportData = KeyboardReportData;

  /* Write Keyboard Report Data */
  Endpoint_Write_Stream_LE(&KeyboardReportData, sizeof(KeyboardReportData), NULL);

  /* Finalize the stream transfer to send the last packet */
  Endpoint_ClearIN();
}