obj-m := lcd1602a_driver.o

ARCH=arm
CROSS_COMPILE=arm-linux-gnueabihf-
KERN_DIR=<PATH TO LINUX SOURCE>

bone: 
	make ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) -C $(KERN_DIR) M=$(PWD) modules

clean: 
	make ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) -C $(KERN_DIR) M=$(PWD) clean
	rm *.elf

help: 
	make ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) -C $(KERN_DIR) M=$(PWD) help

app: gpio.o lcd.o lcd_app.o lcd_app.elf

gpio.o: gpio.c
	$(CROSS_COMPILE)gcc -c -o $@ $^

lcd.o: lcd.c
	$(CROSS_COMPILE)gcc -c -o $@ $^

lcd_app.o: lcd_app.c
	$(CROSS_COMPILE)gcc -c -o $@ $^

lcd_app.elf: gpio.o lcd.o lcd_app.o
	$(CROSS_COMPILE)gcc -o $@ $^
