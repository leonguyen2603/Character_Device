#include <linux/module.h> // Include necessary headers
#include <linux/init.h> // for module initialization and cleanup
#include <linux/fs.h> // for file operations
#include <linux/cdev.h> // for character device operations
#include <linux/uaccess.h> // for user space access functions: 

#define DEVICE_NAME "mychardev" // Define the device name

static int major;

static int my_open(struct inode *inode, struct file *file) // Function to handle opening the device
{
    printk(KERN_INFO "mychardev: Device opened\n");
    return 0;
}

static int my_release(struct inode *inode, struct file *file) // Function to handle releasing the device
{
    printk(KERN_INFO "mychardev: Device released\n");
    return 0;
}

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = my_open,
    .release = my_release,
};

static int __init chardev_init(void) 
{
    major = register_chrdev(0, DEVICE_NAME, &fops); // Register the character device
    printk(KERN_INFO "mychardev: Registered with major number %d\n", major);
    return 0;
}

static void __exit chardev_exit(void) 
{
    unregister_chrdev(major, DEVICE_NAME); // Unregister the character device
    printk(KERN_INFO "mychardev: Unregistered device\n");
}

module_init(chardev_init);
module_exit(chardev_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Leo");
MODULE_DESCRIPTION("Basic Character Device Driver");
