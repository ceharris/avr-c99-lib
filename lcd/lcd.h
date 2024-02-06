/***************************************************************
 * C module for programming the Hitachi HD44780 LCD Controller.
 * Inspired by Arduino's LiquidCrystal library.
 *
 * This module supports only the 4-bit interface for the HD44780,
 * with 5x8 characters.
 *
 * @author Carl Harris
 ***************************************************************/

#ifndef LCD_H
#define LCD_H

#include <stdint.h>

/**
 * A user-supplied function that writes to the LCD in 4-bit 
 * interface mode.
 * @param ctx context byte from the LCD context structure
 * @param r is a byte as follows 
 *     bit 7 = D7 (of the 4-bit parallel interface)
 *     bit 6 = D6
 *     bit 5 = D5
 *     bit 4 = D4 
 *     bit 3 = LCD backlight control (1=on)
 *     bit 2 = E (enable a.k.a. strobe)
 *     bit 1 = RW (always zero)
 *     bit 0 = RS (register select)
 * @return return code (presently unused)
 */
typedef int (*LCDWrite)(uint8_t ctx, uint8_t r);

/**
 * LCD context structure.
 */
typedef struct {
    uint8_t backlight;         /* current state of the backlight */
    uint8_t displayControl;    /* current state of the Display Control flags */
    uint8_t displayMode;       /* current state of the Entry Mode Set flags */
    LCDWrite write;            /* Write function pointer */
    uint8_t ctx;               /* Context for the write function */
} LCD;


/**
 * Initializes the LCD controller using the procedure described
 * as "Initializing by Instruction" in the datasheet.
 *
 * @param lcd LCD context
 * @param address
 */
void lcd_init(LCD* lcd);

/**
 * Turns off the backlight.
 * @param lcd LCD context
 */
void lcd_backlight_off(LCD* lcd);

/**
 * Turns on the backlight.
 * @param lcd LCD context
 */
void lcd_backlight_on(LCD* lcd);

/**
 * Turns off the display using the Display Control command.
 * @param lcd LCD context
 */
void lcd_display_off(LCD* lcd);

/**
 * Turns on the display using the Display Control command.
 * @param lcd LCD context
 */
void lcd_display_on(LCD* lcd);

/**
 * Turns off the blinking cursor using the Display Control command.
 * @param lcd LCD context
 */
void lcd_blink_off(LCD* lcd);

/**
 * Turns on the blinking cursor using the Display Control command.
 * @param lcd LCD context
 */
void lcd_blink_on(LCD* lcd);

/**
 * Turns off the underline cursor using the Display Control command.
 * @param lcd LCD context
 */
void lcd_cursor_off(LCD* lcd);

/**
 * Turns on the underline cursor using the Display Control command.
 * @param lcd LCD context
 */
void lcd_cursor_on(LCD* lcd);

/**
 * Turns off autoscroll (automatic display shift) using the Entry 
 * Mode Set command.
 * @param lcd LCD context
 */
void lcd_autoscroll_off(LCD* lcd);

/**
 * Turns on autoscroll (automatic display shift) using the Entry 
 * Mode Set command.
 * @param lcd LCD context
 */
void lcd_autoscroll_on(LCD* lcd);

/**
 * Sets left-to-right text direction using the Entry Mode Set command.
 * @param lcd LCD context
 */
void lcd_left_to_right(LCD *lcd);

/**
 * Sets right-to-left text direction using the Entry Mode Set command.
 * @param lcd LCD context
 */
void lcd_right_to_left(LCD *lcd);

/**
 * Clears the display using the corresponding command.
 * @param lcd LCD context
 */
void lcd_clear(LCD* lcd);

/**
 * Resets the cursor and display position using the Return Home command.
 * @param lcd LCD context
 */
void lcd_home(LCD* lcd);

/**
 * Positions the cursor to the given column and row, using the Set 
 * DDRAM Address command.
 * @param lcd LCD context
 * @param column zero-based column number
 * @param row zero-based row number
 */
void lcd_goto(LCD* lcd, uint8_t column, uint8_t row);

/**
 * Scrolls (shifts) the display one position to the left using the 
 * Cursor Shift command.
 * @param lcd LCD context
 */
void lcd_scroll_left(LCD* lcd);

/**
 * Scrolls (shifts) the display one position to the right using the 
 * Cursor Shift command.
 * @param lcd LCD context
 */
void lcd_scroll_right(LCD* lcd);

/**
 * Moves the cursor one position to the left using the Cursor Shift 
 * command.
 * @param lcd LCD context
 */
void lcd_cursor_left(LCD* lcd);

/**
 * Moves the cursor one position to the right using the Cursor Shift 
 * command.
 * @param lcd LCD context
 */
void lcd_cursor_right(LCD* lcd);

/**
 * Writes a custom 5x8 character pattern to the controller's CGRAM.
 * @param lcd LCD context
 * @param index index of the character pattern to write (0..7)
 * @param charmap an array of eight 5-bit patterns (in the least 
 *      significant bits) representing the character from top to bottom
 */
void lcd_cg_write(LCD* lcd, uint8_t index, const uint8_t charmap[]);

/**
 * Writes a string to the display at the current cursor position.
 * @param lcd LCD context
 * @param s the string to write
 */
void lcd_puts(LCD* lcd, const char* s);

/**
 * Writes a formatted string to the display at the current cursor position.
 *
 * Available only when the module is compiled with the -DLCD_PRINTF option.
 *
 * @param lcd LCD context
 * @param fmt format string
 * @param ... arguments for the format string
 */
void lcd_printf(LCD* lcd, const char* fmt, ...);

#endif  /* LCD_H */
