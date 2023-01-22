#include "Keyboard.h"
#include "Pin.h"

/** Current Idle period. This is set by the host via a Set Idle HID class
 * request to silence the device's reports for either the entire idle duration,
 * or until the report status changes (e.g. the user presses a key).
 */
static uint16_t IdleCount = 500;

/** Current Idle period remaining. When the IdleCount value is set, this tracks
 * the remaining number of idle milliseconds. This is separate to the IdleCount
 * timer and is incremented and compared as the host may request the current
 * idle period via a Get Idle HID class request, thus its value must be
 * preserved.
 */
static uint16_t IdleMSRemaining = 0;

/** Main program entry point. This routine configures the hardware required by
 * the application, then enters a loop to run the application tasks in sequence.
 */
int main(void) {
  SetupHardware();

  GlobalInterruptEnable();

  for (;;) {
    HID_Task();
    USB_USBTask();
  }
}

/** Configures the board hardware and chip peripherals for the demo's
 * functionality. */
void SetupHardware(void) {
  /* Disable watchdog if enabled by bootloader/fuses */
  MCUSR &= ~(1 << WDRF);
  wdt_disable();

  /* Disable clock division */
  clock_prescale_set(clock_div_1);

  initMatrix();

  /* Hardware Initialization */
  USB_Init();
}

/** Event handler for the USB_Connect event. This indicates that the device is
 * enumerating via the status LEDs and starts the library USB task to begin the
 * enumeration and USB management process.
 */
void EVENT_USB_Device_Connect(void) {}

/** Event handler for the USB_Disconnect event. This indicates that the device
 * is no longer connected to a host via the status LEDs.
 */
void EVENT_USB_Device_Disconnect(void) {}

/** Event handler for the USB_ConfigurationChanged event. This is fired when the
 * host sets the current configuration of the USB device after enumeration, and
 * configures the keyboard device endpoints.
 */
void EVENT_USB_Device_ConfigurationChanged(void) {
  /* Setup HID Report Endpoints */
  Endpoint_ConfigureEndpoint(KEYBOARD_IN_EPADDR, EP_TYPE_INTERRUPT,
                             KEYBOARD_EPSIZE, 1);
  Endpoint_ConfigureEndpoint(KEYBOARD_OUT_EPADDR, EP_TYPE_INTERRUPT,
                             KEYBOARD_EPSIZE, 1);

  /* Turn on Start-of-Frame events for tracking HID report period expiry */
  USB_Device_EnableSOFEvents();
}

/** Event handler for the USB device Start Of Frame event. */
void EVENT_USB_Device_StartOfFrame(void) {
  /* One millisecond has elapsed, decrement the idle time remaining counter if
   * it has not already elapsed */
  if (IdleMSRemaining)
    IdleMSRemaining--;
}

/** Fills the given HID report data structure with the next HID report to send
 * to the host.
 *
 *  \param[out] ReportData  Pointer to a HID report data structure to be filled
 */
void CreateKeyboardReport(USB_KeyboardReport_Data_t *const ReportData) {

  uint8_t UsedKeyCodes = 0;

  /* Clear the report contents */
  memset(ReportData, 0, sizeof(USB_KeyboardReport_Data_t));

  /* Make sent key uppercase by indicating that the left shift key is pressed */
  ReportData->Modifier = HID_KEYBOARD_MODIFIER_LEFTSHIFT;

  ReportData->KeyCode[UsedKeyCodes++] = HID_KEYBOARD_SC_A;
}

/** Sends the next HID report to the host, via the keyboard data endpoint. */
void SendNextReport(void) {
  static USB_KeyboardReport_Data_t PrevKeyboardReportData;
  USB_KeyboardReport_Data_t KeyboardReportData;
  bool SendReport = false;

  /* Create the next keyboard report for transmission to the host */
  // CreateKeyboardReport(&KeyboardReportData);
  getReport(&KeyboardReportData);

  /* Check if the idle period is set and has elapsed */
  if (IdleCount && (!(IdleMSRemaining))) {
    /* Reset the idle time remaining counter */
    IdleMSRemaining = IdleCount;

    /* Idle period is set and has elapsed, must send a report to the host */
    SendReport = true;
  } else {
    /* Check to see if the report data has changed - if so a report MUST be sent
     */
    SendReport = (memcmp(&PrevKeyboardReportData, &KeyboardReportData,
                         sizeof(USB_KeyboardReport_Data_t)) != 0);
  }

  /* Select the Keyboard Report Endpoint */
  Endpoint_SelectEndpoint(KEYBOARD_IN_EPADDR);

  /* Check if Keyboard Endpoint Ready for Read/Write and if we should send a new
   * report */
  if (Endpoint_IsReadWriteAllowed() && SendReport) {
    /* Save the current report data for later comparison to check for changes */
    PrevKeyboardReportData = KeyboardReportData;

    /* Write Keyboard Report Data */
    Endpoint_Write_Stream_LE(&KeyboardReportData, sizeof(KeyboardReportData),
                             NULL);

    /* Finalize the stream transfer to send the last packet */
    Endpoint_ClearIN();
  }
}

/** Function to manage HID report generation and transmission to the host, when
 * in report mode. */
void HID_Task(void) {
  /* Device must be connected and configured for the task to run */
  if (USB_DeviceState != DEVICE_STATE_Configured)
    return;

  /* Send the next keypress report to the host */
  SendNextReport();
}
