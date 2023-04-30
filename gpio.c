
#include "gpio.h"
#include <stdlib.h>
#include <stdio.h>

#define SYSFS_CLASS_DIR_NAME "/sys/class/lcd-16x2/"

#define LCD_GPIO_DIR_NAME "/gpio_dir"

#define GPIO_STR_LEN 100



int gpio_set_value(char *gpio_name, uint8_t value) {
        FILE *fp;
        char buf[GPIO_STR_LEN];

        if (snprintf(buf, GPIO_STR_LEN, SYSFS_CLASS_DIR_NAME "%s/gpio_value", gpio_name) < 0) {
                printf("Could not assign gpio name: %s\n", gpio_name);
                return 0;
        }

        fp = fopen(buf, "w");
        if (fp == NULL) {
                printf("Could not open gpio value: %s\n", buf);
                return 0;
        }

        if (fprintf(fp, "%d\n", value) == 0) {
                printf("Could not write gpio value: %s\n", buf);
                return 0;
        }

        fclose(fp);
        return 1;
}

int gpio_read_value(char *gpio_name, uint8_t *value) {
        FILE *fp;
        char buf[GPIO_STR_LEN];
	int readVal;

        if (snprintf(buf, GPIO_STR_LEN, SYSFS_CLASS_DIR_NAME "%s/gpio_value", gpio_name) < 0) {
                printf("Could not assign gpio name: %s\n", gpio_name);
                return 0;
        }

        fp = fopen(buf, "w");
        if (fp == NULL) {
                printf("Could not open gpio value: %s\n", gpio_name);
                return 0;
        }

	readVal = fgetc(fp);
	printf("Gpio: %s\tvalue: %d\n", gpio_name, readVal);
	*value = (uint8_t)readVal;

        fclose(fp);
        return 1;
}

int gpio_set_direction(char *gpio_name, char *direction) {
	FILE *fp;
	char buf[GPIO_STR_LEN];

	if (snprintf(buf, GPIO_STR_LEN, SYSFS_CLASS_DIR_NAME "%s/gpio_dir", gpio_name) < 0) {
		printf("Could not assign gpio name: %s\n", gpio_name);
		return 0;
	}
	
	fp = fopen(buf, "w");
	if (fp == NULL) {
		printf("Could not open gpio direction: %s\n", gpio_name);
		return 0;
	}

	if (fprintf(fp, "%s\n", direction) == 0) {
		printf("Could not write gpio direction: %s\n", direction);
		return 0;
	}

	fclose(fp);
	return 1;
}
