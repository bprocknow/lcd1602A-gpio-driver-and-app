#include <linux/module.h>
#include <linux/mod_devicetable.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/platform_device.h>
#include <linux/slab.h>
#include <linux/of.h>
#include <linux/gpio/consumer.h>
#include <linux/sysfs.h>

/* --------------- Structure Declarations ---------------- */
struct lcd_private_device_data {
	char label[20];
	struct gpio_desc *desc;	
};

struct lcd_private_driver_data {
	struct class *class;
	struct device **devs;
	int total_devices;
};

/* --------------- Function Declarations ---------------- */
ssize_t label_show(struct device *dev, struct device_attribute *attr, char *buf);
ssize_t gpio_dir_show(struct device *dev, struct device_attribute *attr, char *buf);
ssize_t gpio_dir_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t size);
ssize_t gpio_value_show(struct device *dev, struct device_attribute *attr, char *buf);
ssize_t gpio_value_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t size);
int lcd_probe(struct platform_device *pdev);
int lcd_remove(struct platform_device *pdev);

/* --------------- Variable Definition ---------------- */

struct lcd_private_driver_data lcd_drv_privdata;

struct of_device_id lcd_match_list[] = {
	{.compatible = "lcd_16x2_device"},
	{},
};

struct platform_driver lcd_plat_driver = {
	.probe = lcd_probe,
	.remove = lcd_remove,
	.driver = {
		.name = "lcd-driver",
		.of_match_table = lcd_match_list,
	}
};

DEVICE_ATTR_RO(label);
DEVICE_ATTR_RW(gpio_dir);
DEVICE_ATTR_RW(gpio_value);

struct attribute *attrs[] = {
	&dev_attr_label.attr,
	&dev_attr_gpio_dir.attr,
	&dev_attr_gpio_value.attr,
	NULL
};

struct attribute_group attr_group = {
	.attrs = attrs
};

const struct attribute_group *attr_groups[] = {
	&attr_group,
	NULL
};

/* --------------- Function Definition ---------------- */

ssize_t label_show(struct device *dev, struct device_attribute *attr, char *buf) {
	struct lcd_private_device_data *dev_privdata = dev_get_drvdata(dev);

	return sprintf(buf, "%s\n", dev_privdata->label);
}

ssize_t gpio_dir_show(struct device *dev, struct device_attribute *attr, char *buf) {
	struct lcd_private_device_data *dev_privdata = dev_get_drvdata(dev);
	struct gpio_desc *gpio_desc = dev_privdata->desc;
	int ret;

	int direction = gpiod_get_direction(gpio_desc);

	if (direction < 0) {
		return direction;
	} else if (direction == 0) {
		ret = sprintf(buf, "out\n");
	} else {
		ret = sprintf(buf, "in\n");
	}

	return ret;
}

ssize_t gpio_dir_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t size) {
	struct lcd_private_device_data *dev_privdata = dev_get_drvdata(dev);
	struct gpio_desc *gpio_desc = dev_privdata->desc;
	int ret;

	if (sysfs_streq(buf, "in")) {
		ret = gpiod_direction_input(gpio_desc);
	} else if (sysfs_streq(buf, "out")) {
		ret = gpiod_direction_output(gpio_desc, 0);
	} else {
		return -EINVAL;
	}

	return ret ? : size;
}

ssize_t gpio_value_show(struct device *dev, struct device_attribute *attr, char *buf) {
	struct lcd_private_device_data *dev_privdata = dev_get_drvdata(dev);
	struct gpio_desc *gpio_desc = dev_privdata->desc;
	int ret;

	ret = gpiod_get_value(gpio_desc);
	if (ret < 0) {
		return ret;
	}

	return sprintf(buf, "%d\n", ret);
}

ssize_t gpio_value_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t size) {
	struct lcd_private_device_data *dev_privdata = dev_get_drvdata(dev);
	struct gpio_desc *gpio_desc = dev_privdata->desc;
	int ret;
	long value;

	ret = kstrtol(buf, 0, &value);
	if (ret < 0) {
		return ret;
	}

	gpiod_set_value(gpio_desc, value);

	return size;
}

int lcd_probe(struct platform_device *platdev) {
	int ret;
	int i = 0;
	const char *label_name;
	struct device_node *child_node = NULL;
	struct device *parent_dev = &platdev->dev;
	struct lcd_private_device_data *dev_privdata;

	printk(KERN_INFO "Probing 16x2 lcd\n");
	
	// 1. Find the number of GPIO pins (nodes)
	lcd_drv_privdata.total_devices = of_get_child_count(parent_dev->of_node);
	if (lcd_drv_privdata.total_devices == 0) {
		printk(KERN_ERR "No child devices found for lcd parent\n");
		return -ENODEV;
	}

	dev_info(parent_dev, "Number of gpios: %d\n", lcd_drv_privdata.total_devices);

	// 2. Allocate memory for all children
	lcd_drv_privdata.devs = devm_kzalloc(parent_dev, sizeof(struct device *)*lcd_drv_privdata.total_devices, GFP_KERNEL);
	if (lcd_drv_privdata.devs == NULL) {
		printk(KERN_ERR "Could not allocate memory for lcd driver\n");
		return -ENOMEM;
	}

	// 3. For each child node, initialize lcd devices
	for_each_child_of_node(parent_dev->of_node, child_node) {

		// 4. Allocate memory for the lcd private device data struct
		dev_privdata = devm_kzalloc(parent_dev, sizeof(*dev_privdata), GFP_KERNEL);
		if (dev_privdata == NULL) {
			printk("No memory\n");
			return -ENOMEM;
		}

		// 5. Get label attribute of child
		if (of_property_read_string(child_node, "label", &label_name)) {
			printk(KERN_ERR "Missng label information\n");
			snprintf(dev_privdata->label, sizeof(dev_privdata->label), "uknowngpio-%d", i);
		} else {
			strcpy(dev_privdata->label, label_name);
			dev_info(parent_dev, "GPIO label: %s\n", dev_privdata->label);
		}
		
		// 6. Get GPIO descriptor of child
		dev_privdata->desc = devm_fwnode_get_gpiod_from_child(parent_dev, "bone", &child_node->fwnode, GPIOD_ASIS, dev_privdata->label);
		if (IS_ERR(dev_privdata->desc)) {
			printk(KERN_ERR "Could not get gpio pin for gpio: %s\n", dev_privdata->label);	
			return PTR_ERR(dev_privdata->desc);
		}
		

		// 6. Set GPIO direction to output
		ret = gpiod_direction_output(dev_privdata->desc, 0);
		if (ret) {
			printk(KERN_ERR "Could not set output direction of GPIO: %s\n", dev_privdata->label);
			return ret;
		}

		// 7. Create a struct device for the private data with all the attributes
		lcd_drv_privdata.devs[i] = device_create_with_groups(lcd_drv_privdata.class, parent_dev, 0, dev_privdata, attr_groups, dev_privdata->label);
		if (IS_ERR(lcd_drv_privdata.devs[i])) {
			printk(KERN_ERR "Error creating device\n");
			return PTR_ERR(lcd_drv_privdata.devs[i]);
		}
		
		i++;
	}	

	return 0;
}

int lcd_remove(struct platform_device *pdev) {
	printk(KERN_INFO "Removing lcd driver\n");


	return 0;
}

static int __init lcd_module_init(void) {
	int ret;

	printk(KERN_INFO "Initializing 16x2 lcd device driver\n");

	lcd_drv_privdata.class = class_create(THIS_MODULE, "lcd-16x2");
	if (IS_ERR(lcd_drv_privdata.class)) {
		return PTR_ERR(lcd_drv_privdata.class);
	}
	
	ret = platform_driver_register(&lcd_plat_driver);

	return ret;
}

static void __exit lcd_module_exit(void) {

	printk(KERN_INFO "Uninitializing 16x2 lcd device driver\n");

	platform_driver_unregister(&lcd_plat_driver);
	class_destroy(lcd_drv_privdata.class);
}

module_init(lcd_module_init);
module_exit(lcd_module_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Ben");
MODULE_DESCRIPTION("1602A LCD Driver");
