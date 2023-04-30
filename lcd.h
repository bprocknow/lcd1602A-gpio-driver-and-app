
#include <stdint.h>
#include <stdbool.h>

#define LCD_FIRST_LINE 0
#define LCD_SECOND_LINE 1
#define LCD_MAX_LINES 2

void init_lcd(void);
void lcd_return_home(void);
void lcd_clear_display(void);

/*
 * dispOn:  Indicate whether to turn the display on
 * cursorOn:  Cursor indicates where the next character will be written
 * cursorBlink:  Cursor blinks when on
 */
void lcd_display_on_off_control(bool dispOn, bool cursorOn, bool cursorBlink);

/*
 * Set the address of the cursor to the start of the line argument
 * line: Either 1 or 2.  Select where to move the cursor
 */
void lcd_set_cursor_to_line(int line);

/*
 * Print the string on the display
 */
int lcd_printf(const char *fmt, ...);

/*
 * Shift all the text on the display right or left.
 *
 * shiftValue:	Shift the screen shiftValue times.
 * 		Positive shifts the screen left, negative right.  
 */
void lcd_shift_right_left(int shiftValue);

