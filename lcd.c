
#include "gpio.h"
#include "lcd.h"
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdint.h>
#include <errno.h>

/* -------------------------- Function Declarations --------------------------------- */

/*
 * Sets the lcd function including number of bits to use, number of lines and number of dots
 *
 * isEightBit:  Choose whether to use 8-bits of data or 4-bits of data
 * isTwoLines:  Choose the number of lines to display on the LCD screen, either one or two
 * isFiveByTenDots:  Choose whether to use 5x10 dots for characters or 5x7 dots
 */
static int lcd_function_set(bool isEightBit, bool isTwoLines, bool isFiveByTenDots);

/*
 * Write the 8 values to the data gpio lines
 * Lowest bit to gpio data 0, highest to gpio data 7
 */
static int lcd_set_8_data_value(uint8_t value);

/* -------------------------- Function Definitions --------------------------------- */

static int init_gpios(void) {
	int ret = 1;
	
	ret &= gpio_set_direction(GPIO_RW, GPIO_DIR_OUT);
	ret &= gpio_set_direction(GPIO_RS, GPIO_DIR_OUT);
	ret &= gpio_set_direction(GPIO_EN, GPIO_DIR_OUT);
	ret &= gpio_set_direction(GPIO_DATA1, GPIO_DIR_OUT);
	ret &= gpio_set_direction(GPIO_DATA2, GPIO_DIR_OUT);
	ret &= gpio_set_direction(GPIO_DATA3, GPIO_DIR_OUT);
	ret &= gpio_set_direction(GPIO_DATA4, GPIO_DIR_OUT);
	ret &= gpio_set_direction(GPIO_DATA5, GPIO_DIR_OUT);
	ret &= gpio_set_direction(GPIO_DATA6, GPIO_DIR_OUT);
	ret &= gpio_set_direction(GPIO_DATA7, GPIO_DIR_OUT);
	ret &= gpio_set_direction(GPIO_DATA8, GPIO_DIR_OUT);

	ret &= gpio_set_value(GPIO_RS, LOW_VALUE);
	ret &= gpio_set_value(GPIO_RW, LOW_VALUE);
	ret &= gpio_set_value(GPIO_EN, LOW_VALUE);
	ret &= gpio_set_value(GPIO_DATA1, LOW_VALUE);
	ret &= gpio_set_value(GPIO_DATA2, LOW_VALUE);
	ret &= gpio_set_value(GPIO_DATA3, LOW_VALUE);
	ret &= gpio_set_value(GPIO_DATA4, LOW_VALUE);
	ret &= gpio_set_value(GPIO_DATA5, LOW_VALUE);
	ret &= gpio_set_value(GPIO_DATA6, LOW_VALUE);
	ret &= gpio_set_value(GPIO_DATA7, LOW_VALUE);
	ret &= gpio_set_value(GPIO_DATA8, LOW_VALUE);

	return ret;
}

/*
 * Initialization sequence of 1602A LCD Screen per 
 * http://hades.mech.northwestern.edu/images/f/f7/LCD16x2_HJ1602A.pdf
 */
int init_lcd(void) {
	int ret;

	ret = init_gpios();
	
	usleep(20 * 1000);

	ret &= gpio_set_value(GPIO_RS, LOW_VALUE);
	ret &= gpio_set_value(GPIO_RW, LOW_VALUE);
	ret &= lcd_set_8_data_value(DATA_BIT4 |  DATA_BIT5);

	usleep(5 * 1000);

	ret &= lcd_set_8_data_value(DATA_BIT4 | DATA_BIT5);

	usleep(100);

	ret &= lcd_set_8_data_value(DATA_BIT4 | DATA_BIT5);

	ret &= lcd_set_8_data_value(DATA_BIT5 | DATA_BIT4);
	usleep(50);
	
	// Set 8-bit data length, 2 lcd lines, 5x10 font type
	ret &= lcd_function_set(true, true, true);

	ret &= lcd_clear_display();

	// Entry mode set
	// Initialize cursor and display shift to increment
	ret &= lcd_set_8_data_value(DATA_BIT1 | DATA_BIT2);

	printf("Successfully initialized lcd\n");

	return ret;
}

/*
 * Pulls the enable line high for the LCD to read the other line values
 */
static int lcd_enable(void) {
	int ret = 1;

	ret &= gpio_set_value(GPIO_EN, LOW_VALUE);
	usleep(1);
	ret &= gpio_set_value(GPIO_EN, HIGH_VALUE);
	usleep(1);
	ret &= gpio_set_value(GPIO_EN, LOW_VALUE);
	usleep(100);

	return ret;
}

static int lcd_set_8_data_value(uint8_t value) {
	int ret = 1;

        ret &= gpio_set_value(GPIO_DATA1, value & 0x1);
        ret &= gpio_set_value(GPIO_DATA2, (value >> 1) & 0x1);
        ret &= gpio_set_value(GPIO_DATA3, (value >> 2) & 0x1);
        ret &= gpio_set_value(GPIO_DATA4, (value >> 3) & 0x1);
        ret &= gpio_set_value(GPIO_DATA5, (value >> 4) & 0x1);
        ret &= gpio_set_value(GPIO_DATA6, (value >> 5) & 0x1);
        ret &= gpio_set_value(GPIO_DATA7, (value >> 6) & 0x1);
        ret &= gpio_set_value(GPIO_DATA8, (value >> 7) & 0x1);

	usleep(10);

	// Tell the lcd screen to read the gpio lines
	ret &= lcd_enable();

	return ret;
}

int lcd_return_home(void) {
	int ret = 1;
	
	ret &= gpio_set_value(GPIO_RS, LOW_VALUE);
	ret &= gpio_set_value(GPIO_RW, LOW_VALUE);
	ret &= lcd_set_8_data_value(DATA_BIT1);
	usleep(164 * 10);

	return ret;
}

int lcd_clear_display(void) {
	int ret = 1;
	
	ret &= gpio_set_value(GPIO_RS, LOW_VALUE);
	ret &= gpio_set_value(GPIO_RW, LOW_VALUE);
	ret &= lcd_set_8_data_value(DATA_BIT0);
	usleep(2000);

	return ret;
}

int lcd_display_on_off_control(bool dispOn, bool cursorOn, bool cursorBlink) {
	int ret = 1;
	uint8_t dispOnBit = (int)(dispOn ? DATA_BIT2 : 0);
	uint8_t cursorOnBit = (int)(cursorOn ? DATA_BIT1 : 0);
	uint8_t cursorBlinkBit = (int)(cursorBlink ? DATA_BIT0 : 0);

	ret &= gpio_set_value(GPIO_RS, LOW_VALUE);
	ret &= gpio_set_value(GPIO_RW, LOW_VALUE);
	ret &= lcd_set_8_data_value(DATA_BIT3 | dispOnBit | cursorOnBit | cursorBlinkBit);
	usleep(200);

	return ret;
}

static int lcd_function_set(bool isEightBit, bool isTwoLines, bool isFiveByTenDots) {
	int ret = 1;
	uint8_t dataLenBit = (int)(isEightBit ? DATA_BIT4 : 0);
	uint8_t numLinesBit = (int)(isTwoLines ? DATA_BIT3 : 0);
	uint8_t charFontBit = (int)(isFiveByTenDots ? DATA_BIT2 : 0);

	ret &= gpio_set_value(GPIO_RS, LOW_VALUE);
	ret &= gpio_set_value(GPIO_RW, LOW_VALUE);
	ret &= lcd_set_8_data_value(DATA_BIT5 | dataLenBit | numLinesBit | charFontBit);
	usleep(40);

	return ret;
}

int lcd_shift_right_left(int shiftValue) {
	int ret = 1;
	int i;
	bool isRight = (shiftValue > 0) ? false : true;
	
	if (!shiftValue) 
		return;

	ret &= gpio_set_value(GPIO_RS, LOW_VALUE);
	ret &= gpio_set_value(GPIO_RW, LOW_VALUE);

	for(i = 0; i < abs(shiftValue); i++) {
		ret &= lcd_set_8_data_value(DATA_BIT4 | DATA_BIT3 | DATA_BIT2);
		usleep(40);
	}

	return ret;
}

static int lcd_print_char(uint8_t character) {
	int ret = 1;

	ret &= gpio_set_value(GPIO_RS, HIGH_VALUE);
	ret &= gpio_set_value(GPIO_RW, LOW_VALUE);
	ret &= lcd_set_8_data_value(character);

	usleep(40);

	return ret;
}

int lcd_printf(const char *fmt, ...) {

	char printf_buf[1024];
	va_list args;
	int printed;
	int i;

	if (fmt == NULL) return -EINVAL;

	va_start(args, fmt);
	printed = vsprintf(printf_buf, fmt, args);

	for (i=0; i < printed; i++) {
		if (printf_buf[i] != '\n') {
			if (lcd_print_char(printf_buf[i]) == 0) {
				return -EIO;
			}
		}
	}

	va_end(args);

	return printed;
}

int lcd_set_cursor_to_line(int line) {
	int ret = 1;

	if (line == LCD_FIRST_LINE || line == LCD_SECOND_LINE) {
		// Address to move cursor to 
		uint8_t line_select = (line == LCD_FIRST_LINE) ? 0 : DATA_BIT6;

		ret &= gpio_set_value(GPIO_RS, LOW_VALUE);
		ret &= gpio_set_value(GPIO_RW, LOW_VALUE);
		ret &= lcd_set_8_data_value(DATA_BIT7 | line_select);

	} else {
		printf("Device only supports two lines.  Select line 1 or line 2\n");
		ret = 0;
	}

	return ret;
}
