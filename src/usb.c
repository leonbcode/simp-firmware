#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <stdint.h>

#include "matrix.h"
#include "usb.h"
#include "utils.h"

static uint8_t usb_config_status;
static uint8_t keyboard_protocol;

static uint16_t keyboard_idle_value =
    128;                         // HID Idle setting, how often the device resends unchanging reports, scaling of 4 because of the reg size
static uint8_t current_idle = 0; // Counter that updates based on how many SOFE interrupts have occurred
static uint8_t this_interrupt = 0; // This is not the best way to do it, but it is much more readable than the alternative

static report_t last_report = {0, {0, 0, 0, 0, 0, 0}};

static const uint8_t device_descriptor[] PROGMEM = {
    18, // bLength - The total size of the descriptor
    1,  // bDescriptorType - The type of descriptor - 1 is device
    0x00,
    0x02, // bcdUSB - The USB protcol supported - Refer to USB 2.0 Chapter 9.6.1
    0,    // bDeviceClass - The Device Class, 0 indicating that the HID interface will specify it
    0,    // bDeviceSubClass - 0, HID will specify
    0,    // bDeviceProtocol - No class specific protocols on a device level, HID interface will specify
    32,   // bMaxPacketSize0 - 32 byte packet size; control endpoint was configured in UECFG1X to be 32 bytes
    (idVendor & 255),
    ((idVendor >> 8) & 255), // idVendor - Vendor ID specified by USB-IF (To fit the 2 bytes, the ID is split into LSB and MSB)
    (idProduct & 255),
    ((idProduct >> 8) & 255), // idProduct - The Product ID specified by USB-IF - Split in the same way as idVendor
    0x00,
    0x01, // bcdDevice - Device Version Number
    1,    // iManufacturer - The String Descriptor that has the manufacturer name - Specified by USB 2.0 Table 9-8
    2,    // iProduct - The String Descriptor that has the product name - Specified by USB 2.0 Table 9-8
    0,    // iSerialNumber - The String Descriptor that has the serial number of the product - Specified by USB 2.0 Table 9-8
    1     // bNumConfigurations - The number of configurations of the device, most devices only have one
};

static const uint8_t device_qualifier[] PROGMEM = {
    10,         // bLength - Length of the Device Qualifier Descriptor
    0x06,       // bDescriptorType - Device Qualifier Descriptor Type (0x06)
    0x00, 0x02, // bcdUSB - USB Specification Number (USB 2.0, encoded as 0x0200)
    0x00,       // bDeviceClass - Device Class (0 if specified at the interface level)
    0x00,       // bDeviceSubClass - Device Subclass (0 if specified at the interface level)
    0x00,       // bDeviceProtocol - Device Protocol (0 if specified at the interface level)
    32,         // bMaxPacketSize0 - Maximum packet size for endpoint zero (32 bytes here)
    0,          // bNumConfigurations - Number of configurations (0 if no high-speed support)
    0           // Reserved (must be zero)
};

static const uint8_t language_descriptor[] PROGMEM = {
    4,         // bLength - Length of the Language Descriptor (4 bytes)
    0x03,      // bDescriptorType - String descriptor type
    0x09, 0x04 // wLANGID - 0x0409 for English (US)
};

static const uint8_t manufacturer_string[] PROGMEM = {
    10,                             // bLength - Length of the string descriptor
    0x03,                           // bDescriptorType - String descriptor type
    'D',  0, 'L', 0, 'Y', 0, 'T', 0 // String content in UTF-16LE encoding
};

static const uint8_t product_string[] PROGMEM = {
    10,                             // bLength
    0x03,                           // bDescriptorType
    'S',  0, 'I', 0, 'M', 0, 'P', 0 // Example product name
};

/*  HID Descriptor - The descriptor that gives information about the HID device
        Specification: Device Class Definition for Human Interface Devices (HID)
   6/27/2001 Appendix B - Keyboard Protocol Specification This descriptor was
   written referring to the example descriptor in table E.6
*/
static const uint8_t keyboard_HID_descriptor[] PROGMEM = {
    0x05, 0x01, // Usage Page (Generic Desktop Ctrls)
    0x09, 0x06, // Usage (Keyboard)
    0xA1, 0x01, // Collection (Application)
    0x05, 0x07, //   Usage Page (Kbrd/Keypad)
    0x19, 0xE0, //   Usage Minimum (0xE0)
    0x29, 0xE7, //   Usage Maximum (0xE7)
    0x15, 0x00, //   Logical Minimum (0)
    0x25, 0x01, //   Logical Maximum (1)
    0x75, 0x01, //   Report Size (1)
    0x95, 0x08, //   Report Count (8)
    0x81, 0x02, //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x95, 0x01, //   Report Count (1)
    0x75, 0x08, //   Report Size (8)
    0x81, 0x01, //   Input (Const,Array,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x05, 0x08, //   Usage Page (LEDs)
    0x19, 0x01, //   Usage Minimum (Num Lock)
    0x29, 0x05, //   Usage Maximum (Kana)
    0x95, 0x05, //   Report Count (5)
    0x75, 0x01, //   Report Size (1)

    0x91, 0x02, //   Output (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
    0x95, 0x01, //   Report Count (1)
    0x75, 0x03, //   Report Size (3)
    0x91, 0x01, //   Output (Const,Array,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
    0x95, 0x06, //   Report Count (6)
    0x75, 0x08, //   Report Size (8)
    0x15, 0x00, //   Logical Minimum (0)
    0x25, 0x65, //   Logical Maximum (101)
    0x05, 0x07, //   Usage Page (Kbrd/Keypad)
    0x19, 0x00, //   Usage Minimum (0x00)
    0x29, 0x65, //   Usage Maximum (0x65)
    0x81, 0x00, //   Input (Data,Array,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0xC0,       // End Collection
    // 63 bytes
};

static const uint8_t configuration_descriptor[] PROGMEM = {
    9, // bLength
    2, // bDescriptorType - 2 is device
    (CONFIG_SIZE & 255),
    ((CONFIG_SIZE >> 8) & 255), // wTotalLength - The total length of the descriptor tree
    1,                          // bNumInterfaces - 1 Interface
    1,                          // bConfigurationValue
    0,                          // iConfiguration - We have no string descriptors
    0xC0,                       // bmAttributes - Set the device power source
    50,                         // bMaxPower - 50 x 2mA units = 100mA max power consumption
    // Refer to Table 9-10 for the descriptor structure - Configuration
    // Descriptors have interface descriptors, interface descriptors have
    // endpoint descriptors along with a special HID descriptor
    9,    // bLength
    4,    // bDescriptorType - 4 is interface
    0,    // bInterfaceNumber - This is the 0th and only interface
    0,    // bAlternateSetting - There are no alternate settings
    1,    // bNumEndpoints - This interface only uses one endpoint
    0x03, // bInterfaceClass - 0x03 (specified by USB-IF) is the interface
          // class code for HID
    0x01, // bInterfaceSubClass - 1 (specified by USB-IF) is the constant for
          // the boot subclass - this keyboard can communicate with the BIOS,
          // but is limited to 6KRO, as are most keyboards
    0x01, // bInterfaceProtocol - 0x01 (specified by USB-IF) is the protcol
          // code for keyboards
    0,    // iInterface - There are no string descriptors for this
    // HID Descriptor - Refer to E.4 HID Spec
    9,                                  // bLength
    0x21,                               // bDescriptorType - 0x21 is HID
    0x11, 0x01,                         // bcdHID - HID Class Specification 1.11
    0,                                  // bCountryCode
    1,                                  // bNumDescriptors - Number of HID descriptors
    0x22,                               // bDescriptorType - Type of descriptor
    sizeof(keyboard_HID_descriptor), 0, // wDescriptorLength
    // Endpoint Descriptor - Example can be found in the HID spec table E.5
    7,                            // bLength
    0x05,                         // bDescriptorType
    KEYBOARD_ENDPOINT_NUM | 0x80, // Set keyboard endpoint to IN endpoint, refer to table
    0x03,                         // bmAttributes - Set endpoint to interrupt
    8, 0,                         // wMaxPacketSize - The size of the keyboard banks
    0x01                          // wInterval - Poll for new data 1000/s, or once every ms
};

uint8_t get_usb_config_status() { return usb_config_status; }

void usb_init(void) {
    cli(); // global interrupt disable

    SETBIT(UHWCON, UVREGE); // enable usb pads regulator
    PLLCSR |= 0x12;         // configure 16mHz oscillator

    while (!(PLLCSR & (1 << PLOCK)))
        ; // wait for PLL lock to be achieved

    SETBIT(USBCON, USBE);     // enbale usb controller
    SETBIT(USBCON, OTGPADE);  // enable vbus (usb power) pads
    CLEARBIT(USBCON, FRZCLK); // unfreeze clock

    CLEARBIT(UDCON, LSM);    // enable full speed mode
    CLEARBIT(UDCON, DETACH); // attach device to usb data lines

    SETBIT(UDIEN, EORSTE); // re-enable EORSTE (end of reset) interrupt
    SETBIT(UDIEN, SOFE);   // enable start of frame interrupt

    sei(); // global interrupt enable
}

void usb_transmit(report_t report) {
    UEDATX = report.modifiers;
    UEDATX = 0;
    for (int i = 0; i < 6; i++) {
        UEDATX = report.keys[i];
    }

    UEINTX = 0b00111010;
}

int usb_send(report_t report) {
    if (!usb_config_status)
        return -1; // Why are you even trying
    cli();
    UENUM = KEYBOARD_ENDPOINT_NUM;

    while (!(UEINTX & (1 << RWAL)))
        ; // Wait for banks to be ready

    usb_transmit(report);
    current_idle = 0;
    sei();
    last_report = report;
    return 0;
}

ISR(USB_GEN_vect) {
    uint8_t udint_temp = UDINT; // Capture the current USB interrupt status
    UDINT = 0;

    if (udint_temp & (1 << EORSTI)) { // If end of reset interrupt
        // Configure Control Endpoint
        UENUM = 0;            // Select Endpoint 0, the default control endpoint
        UECONX = (1 << EPEN); // Enable the Endpoint
        UECFG0X = 0;          // Control Endpoint, OUT direction for control endpoint
        UECFG1X |= 0x22;      // 32 byte endpoint, 1 bank, allocate the memory
        usb_config_status = 0;

        if (!(UESTA0X & (1 << CFGOK))) { // Check if endpoint configuration was successful
            return;
        }

        UERST = 1; // Reset Endpoint
        UERST = 0;

        UEIENX = (1 << RXSTPE); // Re-enable the RXSPTE (Receive Setup Packet) Interrupt
        return;
    }

    if ((udint_temp & (1 << SOFI)) && usb_config_status) { // Check for Start Of Frame Interrupt and correct usb configuration, send
                                                           // keypress if a keypress event has not been sent through usb_send
        this_interrupt++;
        if (keyboard_idle_value && (this_interrupt & 3) == 0) { // Scaling by four, trying to save memory
            UENUM = KEYBOARD_ENDPOINT_NUM;
            if (UEINTX & (1 << RWAL)) { // Check if banks are writable
                current_idle++;
                if (current_idle == keyboard_idle_value) { // Have we reached the idle threshold?
                    current_idle = 0;
                    usb_transmit(last_report);
                }
            }
        }
    }
}

ISR(USB_COM_vect) {
    UENUM = 0;
    if (!(UEINTX & (1 << RXSTPI)))
        return;

    uint8_t bmRequestType = UEDATX;
    uint8_t bRequest = UEDATX;
    uint16_t wValue = UEDATX | (UEDATX << 8);
    uint16_t wIndex = UEDATX | (UEDATX << 8);
    uint16_t wLength = UEDATX | (UEDATX << 8);

    DDRC = 0xFF;
    UEINTX &= ~((1 << RXSTPI) | (1 << RXOUTI) | (1 << TXINI)); // Acknowledge

    if (bmRequestType == 0x21) {
        if (bRequest == SET_IDLE) {
            uint8_t idleRate = (wValue >> 8);
            keyboard_idle_value = idleRate * 4;
            UEINTX &= ~(1 << TXINI);

            return;
        } else if (bRequest == SET_REPORT) {
            while (!(UEINTX & (1 << RXOUTI)))
                ;
            UEINTX &= ~(1 << TXINI);
            UEINTX &= ~(1 << RXOUTI);
            return;
        } else if (bRequest == SET_PROTOCOL) {
            keyboard_protocol = wValue >> 8;
            UEINTX &= ~(1 << TXINI);
            return;
        }
    }
    if (bmRequestType == 0xA1) {
        if (bRequest == GET_REPORT) {
            while (!(UEINTX & (1 << TXINI)))
                ;
            usb_transmit(last_report);
            UEINTX &= ~(1 << TXINI);
            return;
        } else if (bRequest == GET_IDLE) {
            while (!(UEINTX & (1 << TXINI)))
                ;
            UEDATX = keyboard_idle_value;
            UEINTX &= ~(1 << TXINI);
            return;
        } else if (bRequest == GET_PROTOCOL) {
            while (!(UEINTX & (1 << TXINI)))
                ;
            UEDATX = keyboard_protocol;
            UEINTX &= ~(1 << TXINI);
            return;
        }
    }
    if (bmRequestType == 0x80 && bRequest == GET_CONFIGURATION) {
        while (!(UEINTX & (1 << TXINI)))
            ;
        UEDATX = usb_config_status;
        UEINTX &= ~(1 << TXINI);
        return;
    }
    if (bmRequestType == 0x00 && bRequest == SET_CONFIGURATION) {
        usb_config_status = wValue;
        UEINTX &= ~(1 << TXINI);
        UENUM = KEYBOARD_ENDPOINT_NUM;
        UECONX = 1;
        UECFG0X = 0b11000001;
        UECFG1X = 0b00000110;
        UERST = 0x1E;
        UERST = 0;
        return;
    }
    if (bmRequestType == 0x00 && bRequest == SET_ADDRESS) {
        UEINTX &= ~(1 << TXINI);
        while (!(UEINTX & (1 << TXINI)))
            ;
        UDADDR = wValue | (1 << ADDEN);
        return;
    }

    if (bRequest == GET_STATUS) {
        while (!(UEINTX & (1 << TXINI)))
            ;
        UEDATX = 0;
        UEDATX = 0;
        UEINTX &= ~(1 << TXINI);
        return;
    }

    if (bRequest == GET_DESCRIPTOR) {
        uint8_t *descriptor;
        uint8_t descriptor_length;

        switch (wValue) {
        case 0x0100:
            descriptor = device_descriptor;
            descriptor_length = pgm_read_byte(descriptor);
            break;
        case 0x0200:
            descriptor = configuration_descriptor;
            descriptor_length = CONFIG_SIZE;
            break;
        case 0x2100:
            descriptor = configuration_descriptor + HID_OFFSET;
            descriptor_length = pgm_read_byte(descriptor);
            break;
        case 0x2200:
            descriptor = keyboard_HID_descriptor;
            descriptor_length = HID_DESCRIPTOR_SIZE;
            break;
        case 0x0300:
            descriptor = language_descriptor;
            descriptor_length = pgm_read_byte(descriptor);
            break;
        case 0x0301:
            descriptor = manufacturer_string;
            descriptor_length = pgm_read_byte(descriptor);
            break;
        case 0x0302:
            descriptor = product_string;
            descriptor_length = pgm_read_byte(descriptor);
            break;
        case 0x0600:
            descriptor = device_qualifier;
            descriptor_length = pgm_read_byte(descriptor);
            break;
        default:
            PORTC = 0xFF;
            UECONX |= (1 << STALLRQ) | (1 << EPEN);
            return;
        }

        uint8_t request_length = wLength > 255 ? 255 : wLength;
        descriptor_length = request_length > descriptor_length ? descriptor_length : request_length;

        while (descriptor_length > 0) {
            while (!(UEINTX & (1 << TXINI)))
                ;
            if (UEINTX & (1 << RXOUTI))
                return;

            uint8_t thisPacket = descriptor_length > 32 ? 32 : descriptor_length;

            for (int i = 0; i < thisPacket; i++) {
                UEDATX = pgm_read_byte(descriptor + i);
            }

            descriptor_length -= thisPacket;
            descriptor += thisPacket;
            UEINTX &= ~(1 << TXINI);
        }
        return;
    }

    PORTC = 0xFF;
    UECONX |= (1 << STALLRQ) | (1 << EPEN);
}