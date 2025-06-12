#include <linux/module.h> // For module macros
#include <linux/init.h> // For module initialization and cleanup
#include <linux/fs.h> // For file operations
#include <linux/cdev.h>  // For character device operations
#include <linux/uaccess.h> // For copy_to_user and copy_from_user

#define DEVICE_NAME "mychardev"

static dev_t dev_number; // Device number      
static struct cdev my_cdev; // Character device structure

static int my_open(struct inode *inode, struct file *file) // Function to handle opening the device
{
    printk(KERN_INFO "mychardev: Device opened\n");
    return 0;
}

static int my_release(struct inode *inode, struct file *file) // Function to handle releasing the device
{
    printk(KERN_INFO "mychardev: Device closed\n");
    return 0;
}

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = my_open,
    .release = my_release,
};

static int __init chardev_init(void) 
{
    int ret;

    ret = alloc_chrdev_region(&dev_number, 0, 1, DEVICE_NAME); // Allocate a device number
    printk(KERN_INFO "Allocated device major: %d minor: %d\n", MAJOR(dev_number), MINOR(dev_number));

    cdev_init(&my_cdev, &fops); // Initialize the cdev structure
    my_cdev.owner = THIS_MODULE; // Set the owner of the cdev

    ret = cdev_add(&my_cdev, dev_number, 1); // Add the character device to the system

    printk(KERN_INFO "Character device registered successfully\n");
    return 0;
}

static void __exit chardev_exit(void) 
{
    cdev_del(&my_cdev); // Delete the character device                           
    unregister_chrdev_region(dev_number, 1); // Unregister the device number      
    printk(KERN_INFO "Character device unregistered\n"); // Cleanup the device
}

module_init(chardev_init);
module_exit(chardev_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Leo");
MODULE_DESCRIPTION("Character Device using alloc_chrdev_region and cdev_add");
