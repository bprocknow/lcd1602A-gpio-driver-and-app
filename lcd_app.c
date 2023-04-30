#include "lcd.h"
#include "gpio.h"
#include <stdio.h>
#include <string.h>

#define MAX_LINE_LEN_W_NULL 17
#define VISIBLE_LCD_CHARS (MAX_LINE_LEN_W_NULL - 1)

static void printf_all_lcd_lines(char text[LCD_MAX_LINES][MAX_LINE_LEN_W_NULL]) {

	if (text == NULL) {
		return;
	}

	lcd_clear_display();

	lcd_set_cursor_to_line(LCD_FIRST_LINE);

	if (text[LCD_FIRST_LINE]) {
		lcd_printf(text[LCD_FIRST_LINE]);
	} else {
		printf("There is no text\n");
	}

	if (LCD_MAX_LINES == 2 && text[LCD_SECOND_LINE]) {
		lcd_set_cursor_to_line(LCD_SECOND_LINE);

		lcd_printf(text[LCD_SECOND_LINE]);
	}
}

// Input user text of max MAX_LINE_LEN_W_NULL characters
void get_user_input(char *userText) {
	int ret;

	memset(userText, '\0', MAX_LINE_LEN_W_NULL);

	printf("Press enter to change lines to write to.  Input Text: ");
	fgets(userText, MAX_LINE_LEN_W_NULL, stdin);
	printf("\n");

	// See if there is more input in the stdin buffer
	if ((strlen(userText) == (VISIBLE_LCD_CHARS)) && 
	  (userText[VISIBLE_LCD_CHARS - 1] != '\n')) {
		// Clear the input buffer
		while(true) {
			ret = getchar();
			if (ret == '\n') break;
			if (ret == EOF) break;
		}
	}
}

int main(void) {
	int shift = 0;
	char chInput;
	char lcdText[LCD_MAX_LINES][MAX_LINE_LEN_W_NULL] = { 0 };
	char inputText[MAX_LINE_LEN_W_NULL];
	int currLine = LCD_FIRST_LINE;

	char *testOutput;

	init_lcd();

	lcd_return_home();

	// Turn the display, cursor and cursor blinking off
	lcd_display_on_off_control(true, false, false);

	// TODO Make sure all return values are checked

	while(1) {
		
		get_user_input(inputText);

		if (strcmp(inputText, "\n") == 0) {
			// Switch to the other line if possible
			currLine = (++currLine % LCD_MAX_LINES);

			printf("Switched lcd to line %d\n", currLine+1);
		} else {
			// Copy the text to the write buffer and print
			memcpy(lcdText[currLine], inputText, MAX_LINE_LEN_W_NULL);
			printf_all_lcd_lines(lcdText);
		}
	}
	return 0;
}
