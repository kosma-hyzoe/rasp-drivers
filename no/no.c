#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include "../include/hmacros.h"

/* Meta Information */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Kosma Marcin Hyżorek");
MODULE_DESCRIPTION("A 'yes' equivalent, outputs 'n' on read repeatedly until killed.");

static dev_t my_device_nr;
static struct class *my_class;
static struct cdev my_device;

static char buffer[255];
static int buffer_pos = 0;
static char NO[2] = "n\n";

#define DRIVER_NAME "no"
#define DRIVER_CLASS DUMMY_CLASS

static ssize_t driver_read(struct file *File, char *user_buffer, size_t count, loff_t *offs) {
	if (buffer_pos == 0)
		return 2 - copy_to_user(user_buffer, NO, 2);

	int to_copy = min(count, buffer_pos);
	int not_copied = copy_to_user(user_buffer, buffer, to_copy);
	return to_copy - not_copied;
}

static ssize_t driver_write(struct file *File, const char *user_buffer, size_t count, loff_t *offs) {
    printk("buffer: %s  count: %d", user_buffer, count); 
	/* if (count == 1) { */
        /* buffer_pos = 0; */
        /* return 0; */
    /* } */

    int to_copy = min(count, sizeof(buffer));
	int not_copied = copy_from_user(buffer, user_buffer, to_copy);
	buffer_pos = to_copy;

	return to_copy - not_copied;
}

static int driver_open(struct inode *device_file, struct file *instance) {
	printk("dev_nr - open was called!\n");
	return 0;
}

/**
 * @brief This function is called, when the device file is opened
 */
static int driver_close(struct inode *device_file, struct file *instance) {
	printk("dev_nr - close was called!\n");
	return 0;
}

static struct file_operations fops = {
	.owner = THIS_MODULE,
	/* .open = driver_open, */
    /* .release = driver_close, */
	.read = driver_read,
	.write = driver_write
};

static int __init ModuleInit(void) {
	/* Allocate a device nr */
	if( alloc_chrdev_region(&my_device_nr, 0, 1, DRIVER_NAME) < 0) {
		printk("Device nr. could not be allocated!\n");
		return -1;
	}
	printk("no - device nr. major: %d, minor: %d was registered.\n",
            GET_MAJOR(my_device_nr), GET_MINOR(my_device_nr));

	/* Create device class */
	if((my_class = class_create(THIS_MODULE, DRIVER_CLASS)) == NULL) {
		printk("Device class can not be created!\n");
		goto ClassError;
	}

	/* create device file */
	if(device_create(my_class, NULL, my_device_nr, NULL, DRIVER_NAME) == NULL) {
		printk("Can not create device file!\n");
		goto FileError;
	}

	/* Initialize device file */
	cdev_init(&my_device, &fops);

	/* Regisering device to kernel */
	if(cdev_add(&my_device, my_device_nr, 1) == -1) {
		printk("Registering of device to kernel failed!\n");
		goto AddError;
	}

	return 0;
AddError:
	device_destroy(my_class, my_device_nr);
FileError:
	class_destroy(my_class);
ClassError:
	unregister_chrdev_region(my_device_nr, 1);
	return -1;
}

/**
 * @brief This function is called, when the module is removed from the kernel
 */
static void __exit ModuleExit(void) {
	cdev_del(&my_device);
	device_destroy(my_class, my_device_nr);
	class_destroy(my_class);
	unregister_chrdev_region(my_device_nr, 1);
	printk("Goodbye, Kernel\n");
}

module_init(ModuleInit);
module_exit(ModuleExit);


