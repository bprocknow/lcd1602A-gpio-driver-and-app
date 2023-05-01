
#include <stdint.h>
#include <stdbool.h>

#define LCD_FIRST_LINE 0
#define LCD_SECOND_LINE 1
#define LCD_MAX_LINES 2

/*
 * Performs the initialization sequence for the lcd screen
 *
 * Returns 0 on error, 1 on success
 */
int init_lcd(void);

/*
 * Postitions the cursor to the home position at the first line and first character
 * Postitions the display postion back to the original position
 *
 * Returns 0 on error, 1 on success
 */
int lcd_return_home(void);

/*
 * Clears all text from the display
 *
 * Returns 0 on error, 1 on success
 */
int lcd_clear_display(void);

/*
 * dispOn:  Indicate whether to turn the display on
 * cursorOn:  Cursor indicates where the next character will be written
 * cursorBlink:  Cursor blinks when on
 * 
 * Returns 0 on error, 1 on success
 */
int lcd_display_on_off_control(bool dispOn, bool cursorOn, bool cursorBlink);

/*
 * Set the address of the cursor to the start of the line argument
 * line: Either 1 or 2.  Select where to move the cursor
 *
 * Returns 0 on error, 1 on success, negative on invalid input
 */
int lcd_set_cursor_to_line(int line);

/*
 * Print the string on the display
 *
 * Returns the number of characters printed on success, negative on error
 */
int lcd_printf(const char *fmt, ...);

/*
 * Shift all the text on the display right or left.
 *
 * shiftValue:	Shift the screen shiftValue times.
 * 		Positive shifts the screen left, negative right.  
 *
 * Returns 0 on error, 1 on success
 */
int lcd_shift_right_left(int shiftValue);

