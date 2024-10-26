//
//  SSD1306.c
//  oled test
//
//  Created by preston sundar on 09/07/17.
//  Copyright © 2017 prestonsundar. All rights reserved.
//

#include "oled/ssd1306.h"
#include "oled/i2c.h"

uint8_t OledLineNum, OledCursorPos;

/***************************************************************************************************
 Local Function Declaration
 ***************************************************************************************************/
void oledSendCommand(uint8_t cmd);
void oledSendStart(uint8_t address);
void oledSendStop(void);
void oledWaitForAck(void);
void oledSendByte(uint8_t ch);
/**************************************************************************************************/

#define FONT_SIZE 5
const unsigned char OledFontTable[][FONT_SIZE] = {

    {0x00, 0x00, 0x00, 0x00, 0x00}, // space
    {0x00, 0x00, 0x2f, 0x00, 0x00}, // !
    {0x00, 0x07, 0x00, 0x07, 0x00}, // "
    {0x14, 0x7f, 0x14, 0x7f, 0x14}, // #
    {0x24, 0x2a, 0x7f, 0x2a, 0x12}, // $
    {0x23, 0x13, 0x08, 0x64, 0x62}, // %
    {0x36, 0x49, 0x55, 0x22, 0x50}, // &
    {0x00, 0x05, 0x03, 0x00, 0x00}, // '
    {0x00, 0x1c, 0x22, 0x41, 0x00}, // (
    {0x00, 0x41, 0x22, 0x1c, 0x00}, // )
    {0x14, 0x08, 0x3E, 0x08, 0x14}, // *
    {0x08, 0x08, 0x3E, 0x08, 0x08}, // +
    {0x00, 0x00, 0xA0, 0x60, 0x00}, // ,
    {0x08, 0x08, 0x08, 0x08, 0x08}, // -
    {0x00, 0x60, 0x60, 0x00, 0x00}, // .
    {0x20, 0x10, 0x08, 0x04, 0x02}, // /

    {0x3E, 0x51, 0x49, 0x45, 0x3E}, // 0
    {0x00, 0x42, 0x7F, 0x40, 0x00}, // 1
    {0x42, 0x61, 0x51, 0x49, 0x46}, // 2
    {0x21, 0x41, 0x45, 0x4B, 0x31}, // 3
    {0x18, 0x14, 0x12, 0x7F, 0x10}, // 4
    {0x27, 0x45, 0x45, 0x45, 0x39}, // 5
    {0x3C, 0x4A, 0x49, 0x49, 0x30}, // 6
    {0x01, 0x71, 0x09, 0x05, 0x03}, // 7
    {0x36, 0x49, 0x49, 0x49, 0x36}, // 8
    {0x06, 0x49, 0x49, 0x29, 0x1E}, // 9

    {0x00, 0x36, 0x36, 0x00, 0x00}, // :
    {0x00, 0x56, 0x36, 0x00, 0x00}, // ;
    {0x08, 0x14, 0x22, 0x41, 0x00}, // <
    {0x14, 0x14, 0x14, 0x14, 0x14}, // =
    {0x00, 0x41, 0x22, 0x14, 0x08}, // >
    {0x02, 0x01, 0x51, 0x09, 0x06}, // ?
    {0x32, 0x49, 0x59, 0x51, 0x3E}, // @

    {0x7C, 0x12, 0x11, 0x12, 0x7C}, // A
    {0x7F, 0x49, 0x49, 0x49, 0x36}, // B
    {0x3E, 0x41, 0x41, 0x41, 0x22}, // C
    {0x7F, 0x41, 0x41, 0x22, 0x1C}, // D
    {0x7F, 0x49, 0x49, 0x49, 0x41}, // E
    {0x7F, 0x09, 0x09, 0x09, 0x01}, // F
    {0x3E, 0x41, 0x49, 0x49, 0x7A}, // G
    {0x7F, 0x08, 0x08, 0x08, 0x7F}, // H
    {0x00, 0x41, 0x7F, 0x41, 0x00}, // I
    {0x20, 0x40, 0x41, 0x3F, 0x01}, // J
    {0x7F, 0x08, 0x14, 0x22, 0x41}, // K
    {0x7F, 0x40, 0x40, 0x40, 0x40}, // L
    {0x7F, 0x02, 0x0C, 0x02, 0x7F}, // M
    {0x7F, 0x04, 0x08, 0x10, 0x7F}, // N
    {0x3E, 0x41, 0x41, 1, 0x3E},    // O
    {0x7F, 0x09, 0x09, 0x09, 0x06}, // P
    {0x3E, 0x41, 0x51, 0x21, 0x5E}, // Q
    {0x7F, 0x09, 0x19, 0x29, 0x46}, // R
    {0x46, 0x49, 0x49, 0x49, 0x31}, // S
    {0x01, 0x01, 0x7F, 0x01, 0x01}, // T
    {0x3F, 0x40, 0x40, 0x40, 0x3F}, // U
    {0x1F, 0x20, 0x40, 0x20, 0x1F}, // V
    {0x3F, 0x40, 0x38, 0x40, 0x3F}, // W
    {0x63, 0x14, 0x08, 0x14, 0x63}, // X
    {0x07, 0x08, 0x70, 0x08, 0x07}, // Y
    {0x61, 0x51, 0x49, 0x45, 0x43}, // Z

    {0x00, 0x7F, 0x41, 0x41, 0x00}, // [
    {0x55, 0xAA, 0x55, 0xAA, 0x55}, // Backslash (Checker pattern)
    {0x00, 0x41, 0x41, 0x7F, 0x00}, // ]
    {0x04, 0x02, 0x01, 0x02, 0x04}, // ^
    {0x40, 0x40, 0x40, 0x40, 0x40}, // _
    {0x00, 0x03, 0x05, 0x00, 0x00}, // `

    {0x20, 0x54, 0x54, 0x54, 0x78}, // a
    {0x7F, 0x48, 0x44, 0x44, 0x38}, // b
    {0x38, 0x44, 0x44, 0x44, 0x20}, // c
    {0x38, 0x44, 0x44, 0x48, 0x7F}, // d
    {0x38, 0x54, 0x54, 0x54, 0x18}, // e
    {0x08, 0x7E, 0x09, 0x01, 0x02}, // f
    {0x18, 0xA4, 0xA4, 0xA4, 0x7C}, // g
    {0x7F, 0x08, 0x04, 0x04, 0x78}, // h
    {0x00, 0x44, 0x7D, 0x40, 0x00}, // i
    {0x40, 0x80, 0x84, 0x7D, 0x00}, // j
    {0x7F, 0x10, 0x28, 0x44, 0x00}, // k
    {0x00, 0x41, 0x7F, 0x40, 0x00}, // l
    {0x7C, 0x04, 0x18, 0x04, 0x78}, // m
    {0x7C, 0x08, 0x04, 0x04, 0x78}, // n
    {0x38, 0x44, 0x44, 0x44, 0x38}, // o
    {0xFC, 0x24, 0x24, 0x24, 0x18}, // p
    {0x18, 0x24, 0x24, 0x18, 0xFC}, // q
    {0x7C, 0x08, 0x04, 0x04, 0x08}, // r
    {0x48, 0x54, 0x54, 0x54, 0x20}, // s
    {0x04, 0x3F, 0x44, 0x40, 0x20}, // t
    {0x3C, 0x40, 0x40, 0x20, 0x7C}, // u
    {0x1C, 0x20, 0x40, 0x20, 0x1C}, // v
    {0x3C, 0x40, 0x30, 0x40, 0x3C}, // w
    {0x44, 0x28, 0x10, 0x28, 0x44}, // x
    {0x1C, 0xA0, 0xA0, 0xA0, 0x7C}, // y
    {0x44, 0x64, 0x54, 0x4C, 0x44}, // z

    {0x00, 0x10, 0x7C, 0x82, 0x00}, // {
    {0x00, 0x00, 0xFF, 0x00, 0x00}, // |
    {0x00, 0x82, 0x7C, 0x10, 0x00}, // }
    {0x00, 0x06, 0x09, 0x09, 0x06}  // ~ (Degrees)
};

/**************************************************************************************************
 void OLED_Init()
 ***************************************************************************************************
 * I/P Arguments:  none
 * Return value : none
 * description  :This function is used to initialize the OLED in the normal
 mode. After initializing the OLED, It clears the OLED and sets the cursor to
 first line first position. .
 **************************************************************************************************/
void OLED_Init(void) {
    i2c_init();

    oledSendCommand(SSD1306_DISPLAY_OFF);
    oledSendCommand(SSD1306_SET_DISPLAY_CLOCK_DIV_RATIO);
    oledSendCommand(0x80);
    oledSendCommand(SSD1306_SET_MULTIPLEX_RATIO);
    oledSendCommand(0x1F);
    oledSendCommand(SSD1306_SET_DISPLAY_OFFSET);
    oledSendCommand(0x0);
    oledSendCommand(SSD1306_SET_START_LINE | 0x0);
    oledSendCommand(SSD1306_CHARGE_PUMP);
    oledSendCommand(0x14);
    oledSendCommand(SSD1306_MEMORY_ADDR_MODE);
    oledSendCommand(0x00);
    oledSendCommand(SSD1306_SET_SEGMENT_REMAP);
    oledSendCommand(SSD1306_COM_SCAN_DIR_INC);
    oledSendCommand(SSD1306_SET_COM_PINS);
    oledSendCommand(0x02);
    oledSendCommand(SSD1306_SET_CONTRAST_CONTROL);
    oledSendCommand(0xCF);
    oledSendCommand(SSD1306_SET_PRECHARGE_PERIOD);
    oledSendCommand(0xF1);
    oledSendCommand(SSD1306_SET_VCOM_DESELECT);
    oledSendCommand(0x40);
    oledSendCommand(SSD1306_DISPLAY_ALL_ON_RESUME);
    oledSendCommand(SSD1306_NORMAL_DISPLAY);
    oledSendCommand(SSD1306_DISPLAY_ON);

    OLED_Clear(); /* Clear the complete LCD during init */
}

/***************************************************************************************************
 void OLED_DisplayChar( char ch)
 ****************************************************************************************************
 * I/P Arguments: ASCII value of the char to be displayed.
 * Return value    : none
 * description  : This function sends a character to be displayed on LCD.
 Any valid ascii value can be passed to display respective character
 ****************************************************************************************************/
void OLED_DisplayChar(uint8_t ch) {
    uint8_t dat, i = 0;

    if (((OledCursorPos + FONT_SIZE) >= 128) || (ch == '\n')) {
        /* If the cursor has reached to end of line on page1
         OR NewLine command is issued Then Move the cursor to next line */
        OLED_GoToNextLine();
    }
    if (ch != '\n') /* TODO */
    {
        ch = ch - 0x20; // As the lookup table starts from Space(0x20)

        while (1) {
            dat = OledFontTable[ch][i]; /* Get the data to be displayed for
                                           LookUptable*/

            oledSendByte(dat); /* Display the data and keep track of cursor */
            OledCursorPos++;

            i++;

            if (i == FONT_SIZE) /* Exit the loop if End of char is encountered */
            {
                oledSendByte(0x00); /* Display the data and keep track of cursor */
                OledCursorPos++;
                break;
            }
        }
    }
}

/***************************************************************************************************
 void OLED_DisplayString(char *ptr_stringPointer_u8)
 ****************************************************************************************************
 * I/P Arguments: String(Address of the string) to be displayed.
 * Return value    : none
 * description  :
 This function is used to display the ASCII string on the lcd.
 1.The ptr_stringPointer_u8 points to the first char of the string
 and traverses till the end(NULL CHAR)and displays a char each time.
 ****************************************************************************************************/
void OLED_DisplayString(uint8_t *ptr) {
    while (*ptr)
        OLED_DisplayChar(*ptr++);
}

/***************************************************************************************************
 void OLED_Clear()
 ****************************************************************************************************
 * I/P Arguments: none.
 * Return value    : none
 * description  :This function clears the LCD and moves the cursor to beginning
 of first line
 ****************************************************************************************************/
void OLED_Clear(void) {
    int i;

    oledSendCommand(SSD1306_SET_COLUMN_ADDR);
    oledSendCommand(0);
    oledSendCommand(127);

    oledSendCommand(SSD1306_SET_PAGE_ADDR);
    oledSendCommand(0);
    oledSendCommand(7);

    oledSendStart(SSD1306_ADDRESS);
    oledSendByte(SSD1306_DATA_CONTINUE);

    for (i = 0; i < 512; i++) // Write Zeros to clear the display
    {
        oledSendByte(0);
    }

    oledSendCommand(SSD1306_SET_COLUMN_ADDR);
    oledSendCommand(0);
    oledSendCommand(127);

    oledSendCommand(SSD1306_SET_PAGE_ADDR);
    oledSendCommand(0);
    oledSendCommand(7);

    oledSendStart(SSD1306_ADDRESS);
    oledSendByte(SSD1306_DATA_CONTINUE);
}

/***************************************************************************************************
 void OLED_GoToLine(uint8_t v_lineNumber_u8)
 ****************************************************************************************************
 * I/P Arguments: uint8_t: Line number(0-7).
 * Return value    : none
 * description  :This function moves the Cursor to beginning of the specified
 line. If the requested line number is out of range, it will not move the
 cursor. Note: The line numbers run from 0 to 7
 ****************************************************************************************************/
void OLED_GoToLine(uint8_t lineNumber) {
    if (lineNumber < 8) { /* If the line number is within range
                           then move it to specified line and keep track*/
        OledLineNum = lineNumber;
        OLED_SetCursor(OledLineNum, 0);
    }
}

/***************************************************************************************************
 void  OLED_GoToNextLine()
 ****************************************************************************************************
 * I/P Arguments: none
 * Return value    : none
 * description  :This function moves the Cursor to beginning of the next line.
 If the cursor is on last line and NextLine command is issued then
 it will move the cursor to first line.
 ****************************************************************************************************/
void OLED_GoToNextLine(void) {
    /*Increment the current line number.
     In case it exceeds the limit, rool it back to first line */
    OledLineNum++;
    OledLineNum = OledLineNum & 0x07;
    OLED_SetCursor(OledLineNum, 0); /* Finally move it to next line */
}

/***************************************************************************************************
 void OLED_SetCursor(char v_lineNumber_u8,char v_charNumber_u8)
 ****************************************************************************************************
 * I/P Arguments: char row,char col
 row -> line number(line1=1, line2=2),
 For 2line LCD the I/P argument should be either 1 or 2.
 col -> char number.
 For 16-char LCD the I/P argument should be between 0-15.
 * Return value    : none
 * description  :This function moves the Cursor to specified position
 Note:If the Input(Line/Char number) are out of range
 then no action will be taken
 ****************************************************************************************************/
void OLED_SetCursor(uint8_t lineNumber, uint8_t cursorPosition) {
    /* Move the Cursor to specified position only if it is in range */
    if ((lineNumber <= C_OledLastLine_U8) && (cursorPosition <= 127)) {
        OledLineNum = lineNumber;       /* Save the specified line number */
        OledCursorPos = cursorPosition; /* Save the specified cursor position */

        oledSendCommand(SSD1306_SET_COLUMN_ADDR);
        oledSendCommand(cursorPosition);
        oledSendCommand(127);

        oledSendCommand(SSD1306_SET_PAGE_ADDR);
        oledSendCommand(lineNumber);
        oledSendCommand(7);

        oledSendStart(SSD1306_ADDRESS);
        oledSendByte(SSD1306_DATA_CONTINUE);
    }
}

/***************************************************************************************************
 void OLED_DisplayLogo(char *ptr_Logo)
 ****************************************************************************************************
 * I/P Arguments   : Array Bit Map(Address of Bit Map) to be displayed.
 * Return value    : none
 * description  :
 This function is used to display the logo for using the bit map.
 User should pass the array/address of bitmap for diplay the logo.
 User can enable/disable the inversion of the dislpay by using the below
 functions. OLED_EnableInversion/OLED_DisableInversion
 ****************************************************************************************************/
void OLED_DisplayFrame(uint8_t *buffer) {
    int i;

    OLED_SetCursor(0, 0);

    oledSendStart(SSD1306_ADDRESS);
    oledSendByte(SSD1306_DATA_CONTINUE);

    for (i = 0; i < 512; i++) // Send data
    {
        oledSendByte(buffer[i]);
    }
}

void OLED_EnableInversion(void) { oledSendCommand(SSD1306_INVERT_DISPLAY); }

void OLED_DisableInversion(void) { oledSendCommand(SSD1306_NORMAL_DISPLAY); }

/***************************************************************************************************
 void OLED_SetBrightness(uint8_t brightnessValue)
 ****************************************************************************************************
 * I/P Arguments   : uint8_t: brightness level from 0-255.
 * Return value    : none
 * description  :
 This function is used to adjust the contrast/Brightness of the OLED.
 ****************************************************************************************************/
void OLED_SetBrightness(uint8_t brightnessValue) {
    oledSendCommand(SSD1306_SET_CONTRAST_CONTROL);
    oledSendCommand(brightnessValue);
}

/********************************************************************************
 Local FUnctions for sending the command/data
 ********************************************************************************/

void oledSendStart(uint8_t address) {

    i2c_tx_start(MASTER_TRANSMITTER);
    i2c_tx_address(address);
}

void oledSendStop(void) { i2c_tx_stop(); }

void oledSendByte(uint8_t ch) { i2c_tx_byte(ch); }

void oledSendCommand(uint8_t cmd) {
    // oledSendStart(SSD1306_ADDRESS<<1);
    oledSendStart(SSD1306_ADDRESS);
    oledSendByte(SSD1306_COMMAND);
    oledSendByte(cmd);
    oledSendStop();
}

/*****************************************************************************************************/
