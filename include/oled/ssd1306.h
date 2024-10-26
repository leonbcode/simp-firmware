#pragma once

#include <stdint.h>

/***************************************************************************************************
 Macros to find the mod of a number
 ****************************************************************************************************/
#define util_GetMod8(dividend, divisor) (uint8_t)(dividend - (divisor * (uint8_t)(dividend / divisor)))
#define util_GetMod16(dividend, divisor) (uint16_t)(dividend - (divisor * (uint16_t)(dividend / divisor)))
#define util_GetMod32(dividend, divisor) (uint32_t)(dividend - (divisor * (uint32_t)(dividend / divisor)))
/***************************************************************************************************/

/***************************************************************************************************
 Macros for Dec2Ascii,Hec2Ascii and Acsii2Hex conversion
 *****************************************************************************************************/
#define util_Dec2Ascii(Dec) ((Dec) + 0x30)
#define util_Ascii2Dec(Asc) ((Asc)-0x30)
#define util_Hex2Ascii(Hex) (((Hex) > 0x09) ? ((Hex) + 0x37) : ((Hex) + 0x30))
#define util_Ascii2Hex(Asc) (((Asc) > 0x39) ? ((Asc)-0x37) : ((Asc)-0x30))
#define util_GetBitStatus(x, bit) (((x) & (util_GetBitMask(bit))) != 0u)
#define util_GetBitMask(bit) (1 << (bit))
/***************************************************************************************************/

/******************************************************************************
 standard defs
 ******************************************************************************/

typedef signed char sint8_t;
typedef unsigned char uint8_t;
typedef signed int sint16_t;
typedef signed long int sint32_t;

#define C_BINARY_U8 2u
#define C_DECIMAL_U8 10u
#define C_HEX_U8 16u

#define C_SINT8_MAX 0x7F
#define C_SINT8_MIN -128

#define C_UINT8_MAX 0xFFu
#define C_UINT8_MIN 0x00u

#define C_SINT16_MAX 32767
#define C_SINT16_MIN -32768

#define C_UINT16_MAX 0xFFFFu
#define C_UINT16_MIN 0x00u

#define C_SINT32_MAX 2147483647
#define C_SINT32_MIN -2147483648

#define C_UINT32_MAX 0xFFFFFFFFu
#define C_UINT32_MIN 0x00u

typedef enum { E_BINARY = 2, E_DECIMAL = 10, E_HEX = 16 } NumericSystem_et;

/******************************************************************************
 SSD1306 ID and Command List
 ******************************************************************************/
#define SSD1306_ADDRESS 0x3C

#define SSD1306_COMMAND 0x00
#define SSD1306_DATA 0xC0
#define SSD1306_DATA_CONTINUE 0x40

#define SSD1306_SET_CONTRAST_CONTROL 0x81
#define SSD1306_DISPLAY_ALL_ON_RESUME 0xA4
#define SSD1306_DISPLAY_ALL_ON 0xA5
#define SSD1306_NORMAL_DISPLAY 0xA6
#define SSD1306_INVERT_DISPLAY 0xA7
#define SSD1306_DISPLAY_OFF 0xAE
#define SSD1306_DISPLAY_ON 0xAF
#define SSD1306_NOP 0xE3

#define SSD1306_HORIZONTAL_SCROLL_RIGHT 0x26
#define SSD1306_HORIZONTAL_SCROLL_LEFT 0x27
#define SSD1306_HORIZONTAL_SCROLL_VERTICAL_AND_RIGHT 0x29
#define SSD1306_HORIZONTAL_SCROLL_VERTICAL_AND_LEFT 0x2A
#define SSD1306_DEACTIVATE_SCROLL 0x2E
#define SSD1306_ACTIVATE_SCROLL 0x2F
#define SSD1306_SET_VERTICAL_SCROLL_AREA 0xA3

#define SSD1306_SET_LOWER_COLUMN 0x00
#define SSD1306_SET_HIGHER_COLUMN 0x10
#define SSD1306_MEMORY_ADDR_MODE 0x20
#define SSD1306_SET_COLUMN_ADDR 0x21
#define SSD1306_SET_PAGE_ADDR 0x22

#define SSD1306_SET_START_LINE 0x40
#define SSD1306_SET_SEGMENT_REMAP 0xA0
#define SSD1306_SET_MULTIPLEX_RATIO 0xA8
#define SSD1306_COM_SCAN_DIR_INC 0xC0
#define SSD1306_COM_SCAN_DIR_DEC 0xC8
#define SSD1306_SET_DISPLAY_OFFSET 0xD3
#define SSD1306_SET_COM_PINS 0xDA
#define SSD1306_CHARGE_PUMP 0x8D

#define SSD1306_SET_DISPLAY_CLOCK_DIV_RATIO 0xD5
#define SSD1306_SET_PRECHARGE_PERIOD 0xD9
#define SSD1306_SET_VCOM_DESELECT 0xDB
/****************************************************************************/

/***************************************************************************************************
 Commonly used LCD macros/Constants
 ***************************************************************************************************/
#define C_DefaultDigits 10

#define C_OledFirstLine_U8 0x00u
#define C_OledLastLine_U8 0x03u

#define C_FirstLineAddress_U8 0xB8
#define C_LastLineAddress_U8 0xBF

#define C_DisplayDefaultDigits_U8 0xffu  // Will display the exact digits in the number
#define C_MaxDigitsToDisplay_U8 10u      // Max decimal/hexadecimal digits to be displayed
#define C_NumOfBinDigitsToDisplay_U8 16u // Max bits of a binary number to be displayed
#define C_MaxDigitsToDisplayUsingPrintf_U8                                                                             \
    C_DisplayDefaultDigits_U8 /* Max dec/hexadecimal digits to be displayed                                            \
                                 using printf */

#define C_MaxBarGraphs_U8 5

/***************************************************************************************************
 Function Prototypes
 ***************************************************************************************************/
void OLED_Init(void);
void OLED_DisplayChar(uint8_t ch);
void OLED_DisplayString(uint8_t *);
void OLED_Clear(void);
void OLED_GoToLine(uint8_t);
void OLED_GoToNextLine(void);
void OLED_SetCursor(uint8_t lineNumber, uint8_t CursorPosition);
void OLED_DisplayFrame(uint8_t *buffer);
void OLED_EnableInversion(void);
void OLED_DisableInversion(void);
/**************************************************************************************************/