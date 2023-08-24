#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/uaccess.h>

#define DRIVER_NAME "buffer_dev"
#define DRIVER_CLASS "dummy"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Kosma Marcin Hyzorek");
MODULE_DESCRIPTION("Testing dev numbers");

static dev_t my_device_nr;
static struct class *my_class;
static struct cdev my_device;

static char buffer[255];
static int buffer_pointer = 0;

static ssize_t read(struct file *fp, char *user_buffer, size_t count,
                    loff_t *offs)
{
    int to_copy = min(count, buffer_pointer);
    int not_copied = copy_to_user(user_buffer, buffer, to_copy);

    return to_copy - not_copied;
}

static int write(struct file *fp, const char *user_buffer, size_t count,
                 loff_t *offs)
{
    int to_copy = min(count, sizeof(buffer));
    int not_copied = copy_from_user(buffer, user_buffer, to_copy);

    buffer_pointer = to_copy;
    return to_copy - not_copied;
}

static int driver_open(struct inode *device_file, struct file *instance)
{
    printk("custom_dev opened\n");
    return 0;
}

static int driver_close(struct inode *device_file, struct file *instance)
{
    printk("custom_dev closed\n");
    return 0;
}

static struct file_operations fops = {
    .owner = THIS_MODULE, .open = driver_open, .release = driver_close,
    .read = read, .write = write};

static int major;

static int __init ModuleInit(void)
{
    int major = register_chrdev(0, "custom_dev", &fops);
    if (major < 0) {
        printk("Failed to register char device");
        return -1;
    }
    /* Create device class */
    if ((my_class = class_create(THIS_MODULE, DRIVER_CLASS)) == NULL) {
        printk("Device class can not be created!\n");
        goto ClassError;
    }

    /* create device file */
    if (!device_create(my_class, NULL, my_device_nr, NULL, DRIVER_NAME)) {
        printk("Can not create device file!\n");
        goto FileError;
    }
    printk("my_class: %s", my_class);

    /* Initialize device file */
    cdev_init(&my_device, &fops);

    /* Regisering device to kernel */
    if (cdev_add(&my_device, my_device_nr, 1) == -1) {
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
static void __exit ModuleExit(void) { unregister_chrdev(major, "custom_dev"); }

module_init(ModuleInit);
module_exit(ModuleExit);
