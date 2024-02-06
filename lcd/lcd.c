/***************************************************************
 * C module for programming the Hitachi HD44780 LCD Controller.
 * Inspired by Arduino's LiquidCrystal library.
 *
 * This module supports only the 4-bit interface for the HD44780,
 * with 5x8 characters.
 *
 * @author Carl Harris
 ***************************************************************/

#ifdef LCD_PRINTF
#include <stdarg.h>
#include <stdio.h>
#endif

#include <util/delay.h>
#define delay_us(us)  (_delay_us(us))

#include "lcd.h"

/* LCD to GPIO interface definitions */
#define LCD_RS 0x1      /* register select (data=1/command=0) */
#define LCD_RW 0x2      /* read=1/write=0 */
#define LCD_E  0x4      /* enable (strobe) */
#define LCD_BL 0x8      /* backlight (on=1/off=0) */
                        /* upper nibble is D4..D7 */

/* Controller interface flags */
#define LCD_COMMAND 0
#define LCD_DATA LCD_RS
#define LCD_WRITE 0
#define LCD_READ LCD_RW
#define LCD_BACKLIGHT_OFF 0
#define LCD_BACKLIGHT_ON LCD_BL

/* Controller command bit patterns */
#define LCD_CLEARDISPLAY 0x1
#define LCD_RETURNHOME 0x2
#define LCD_ENTRYMODESET 0x4
#define LCD_DISPLAYCONTROL 0x8
#define LCD_CURSORSHIFT 0x10
#define LCD_FUNCTIONSET 0x20
#define LCD_SETCGRAMADDR 0x40
#define LCD_SETDDRAMADDR 0x80

/* Entry Mode Set flags */
#define LCD_ENTRYSHIFT 0x1
#define LCD_ENTRYLEFT 0x2

/* Display Control flags */
#define LCD_DISPLAY_ON 0x4
#define LCD_CURSOR_ON 0x2
#define LCD_BLINK_ON 0x1

/* Cursor Shift flags */
#define LCD_CURSORMOVE 0
#define LCD_DISPLAYMOVE 0x8
#define LCD_MOVELEFT 0
#define LCD_MOVERIGHT 0x4

/* Function Set flags */
#define LCD_1LINE 0
#define LCD_2LINE 0x8
#define LCD_5X8 0
#define LCD_5X10 0x4

#ifdef LCD_PRINTF
#ifndef LCD_PRINT_BUFFER_SIZE
#define LCD_PRINT_BUFFER_SIZE 80
#endif
#endif

#define lcd_write(lcd, b) (lcd->write(lcd->ctx, b))

/**
 * Writes a nibble (4-bits) to the LCD controller.
 * @param lcd LCD context
 * @param b byte whose lower 4-bits is the nibble to write
 * @param rs_flag state for the register select (RS) pin
 */
static void lcd_write_nibble(LCD* lcd, uint8_t b, uint8_t rs_flag) {
  uint8_t output = (b << 4) | lcd->backlight | rs_flag;
  lcd_write(lcd, output);
  delay_us(1);
  lcd_write(lcd, output | LCD_E);
  delay_us(1);
  lcd_write(lcd, output);
  delay_us(100);
}

/**
 * Writes a command byte to the LCD controller.
 * @param lcd LCD context
 * @param c byte containing the command to write
 */
void lcd_write_command(LCD* lcd, uint8_t c) {
  lcd_write_nibble(lcd, c >> 4, LCD_COMMAND);
  lcd_write_nibble(lcd, c & 0xf, LCD_COMMAND);
}

/**
 * Writes a data byte to the LCD controller.
 * @param lcd LCD context
 * @param d data byte to write
 */
void lcd_write_data(LCD* lcd, uint8_t d) {
  lcd_write_nibble(lcd, d >> 4, LCD_DATA);
  lcd_write_nibble(lcd, d & 0xf, LCD_DATA);
}

void lcd_init(LCD* lcd) {
  lcd->backlight = LCD_BL;
  delay_us(50000);
  lcd->write(lcd->ctx, 0);

  lcd_write_nibble(lcd, 0x3, 0);
  delay_us(4500);
  lcd_write_nibble(lcd, 0x3, 0);
  delay_us(4500);
  lcd_write_nibble(lcd, 0x3, 0);
  delay_us(150);
  lcd_write_nibble(lcd, 0x2, 0);

  lcd_write_command(lcd, LCD_FUNCTIONSET | LCD_2LINE);
  lcd->displayControl = LCD_DISPLAY_ON;
  lcd_write_command(lcd, LCD_DISPLAYCONTROL | lcd->displayControl);
  lcd->displayMode = LCD_ENTRYLEFT;
  lcd_write_command(lcd, LCD_ENTRYMODESET | lcd->displayMode);
  lcd_write_command(lcd, LCD_CLEARDISPLAY);
  delay_us(2000);
}

void lcd_backlight_off(LCD* lcd) {
  lcd->backlight = LCD_BACKLIGHT_OFF;
  lcd_write_command(lcd, 0);
}

void lcd_backlight_on(LCD* lcd) {
  lcd->backlight = LCD_BACKLIGHT_ON;
  lcd_write_command(lcd, 0);
}

void lcd_display_off(LCD* lcd) {
  lcd->displayControl &= ~LCD_DISPLAY_ON;
  lcd_write_command(lcd, LCD_DISPLAYCONTROL | lcd->displayControl);
}

void lcd_display_on(LCD* lcd) {
  lcd->displayControl |= LCD_DISPLAY_ON;
  lcd_write_command(lcd, LCD_DISPLAYCONTROL | lcd->displayControl);
}

void lcd_blink_off(LCD* lcd) {
  lcd->displayControl &= ~LCD_BLINK_ON;
  lcd_write_command(lcd, LCD_DISPLAYCONTROL | lcd->displayControl);
}

void lcd_blink_on(LCD* lcd) {
  lcd->displayControl |= LCD_BLINK_ON;
  lcd_write_command(lcd, LCD_DISPLAYCONTROL | lcd->displayControl);
}

void lcd_cursor_off(LCD* lcd) {
  lcd->displayControl &= ~LCD_CURSOR_ON;
  lcd_write_command(lcd, LCD_DISPLAYCONTROL | lcd->displayControl);
}

void lcd_cursor_on(LCD* lcd) {
  lcd->displayControl |= LCD_CURSOR_ON;
  lcd_write_command(lcd, LCD_DISPLAYCONTROL | lcd->displayControl);
}

void lcd_autoscroll_off(LCD* lcd) {
  lcd->displayMode &= ~LCD_ENTRYSHIFT;
  lcd_write_command(lcd, LCD_ENTRYMODESET | lcd->displayMode);
}

void lcd_autoscroll_on(LCD* lcd) {
  lcd->displayMode |= LCD_ENTRYSHIFT;
  lcd_write_command(lcd, LCD_ENTRYMODESET | lcd->displayMode);
}

void lcd_left_to_right(LCD* lcd) {
  lcd->displayMode |= LCD_ENTRYLEFT;
  lcd_write_command(lcd, LCD_ENTRYMODESET | lcd->displayMode);
}

void lcd_right_to_left(LCD* lcd) {
  lcd->displayMode &= ~LCD_ENTRYLEFT;
  lcd_write_command(lcd, LCD_ENTRYMODESET | lcd->displayMode);
}

void lcd_clear(LCD* lcd) {
  lcd_write_command(lcd, LCD_CLEARDISPLAY);
  delay_us(2000);
}

void lcd_home(LCD* lcd) {
  lcd_write_command(lcd, LCD_RETURNHOME);
  delay_us(2000);
}

void lcd_goto(LCD* lcd, uint8_t x, uint8_t y) {
  uint8_t offset = ((y & 0x1) << 6) + (x & 0x3f);
  lcd_write_command(lcd, LCD_SETDDRAMADDR | offset);
}

void lcd_scroll_left(LCD* lcd) {
  lcd_write_command(lcd, LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVELEFT);
}

void lcd_scroll_right(LCD* lcd) {
  lcd_write_command(lcd, LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVERIGHT);
}

void lcd_cursor_left(LCD* lcd) {
  lcd_write_command(lcd, LCD_CURSORSHIFT | LCD_CURSORMOVE | LCD_MOVELEFT);
}

void lcd_cursor_right(LCD* lcd) {
  lcd_write_command(lcd, LCD_CURSORSHIFT | LCD_CURSORMOVE | LCD_MOVERIGHT);
}

void lcd_cg_write(LCD* lcd, uint8_t index, const uint8_t charmap[]) {
  uint8_t address = (index & 0x7) << 3;
  lcd_write_command(lcd, LCD_SETCGRAMADDR | address);
  for (uint8_t i = 0; i < 8; i++) {
    lcd_write_data(lcd, charmap[i]);
  } 
}

void lcd_puts(LCD* lcd, const char* s) {
  while (*s != 0) {
    lcd_write_data(lcd, *s);
    s++;
  }
}

#ifdef LCD_PRINTF
void lcd_printf(LCD* lcd, const char* fmt, ...) {
  char buf[LCD_PRINT_BUFFER_SIZE];
  va_list args;
  va_start(args, fmt);
  vsnprintf(buf, sizeof(buf), fmt, args);
  va_end(args);
  lcd_puts(lcd, buf);
}
#endif

