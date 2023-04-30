
#include <stdint.h>

#define GPIO_RS 	"gpio2_22"
#define GPIO_RW 	"gpio2_24"
#define GPIO_EN 	"gpio2_23"
#define GPIO_DATA1	"gpio2_7"
#define GPIO_DATA2	"gpio2_6"
#define GPIO_DATA3	"gpio2_9"
#define GPIO_DATA4	"gpio2_8"
#define GPIO_DATA5	"gpio2_11"
#define GPIO_DATA6	"gpio2_10"
#define GPIO_DATA7	"gpio2_13"
#define GPIO_DATA8	"gpio2_12"

#define DATA_BIT0 1
#define DATA_BIT1 (1 << 1)
#define DATA_BIT2 (1 << 2)
#define DATA_BIT3 (1 << 3)
#define DATA_BIT4 (1 << 4)
#define DATA_BIT5 (1 << 5)
#define DATA_BIT6 (1 << 6)
#define DATA_BIT7 (1 << 7)

#define LOW_VALUE 0
#define HIGH_VALUE 1

#define GPIO_DIR_IN "in"
#define GPIO_DIR_OUT "out"

int gpio_set_direction(char *gpio_name, char *direction);

/*
 * Sets the 8 bits of data with the lowest 8 bits of the inputted value argument
 */
int gpio_set_value(char *gpio_name, uint8_t value);

int gpio_read_value(char *gpio_name, uint8_t *value);

