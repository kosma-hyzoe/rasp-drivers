#include <linux/fs.h>
#include <linux/init.h>
#include <linux/module.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Kosma Marcin Hyzorek");
MODULE_DESCRIPTION("Testing dev numbers");

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
    .owner = THIS_MODULE,
    .open = driver_open,
    .release = driver_close
};

int major;

static int __init ModuleInit(void)
{
    int retval = register_chrdev(0, "custom_dev", &fops);
    if (retval < 0) {
        printk("Failed to register char device");
        return -1;
    }
    major = retval;
    return 0;
}

static void __exit ModuleExit(void)
{
    unregister_chrdev(major, "custom_dev");
}

module_init(ModuleInit);
module_exit(ModuleExit);
