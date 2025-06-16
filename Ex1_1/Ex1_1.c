#include <linux/module.h> // module_init, module_exit
#include <linux/init.h> // functions: module_init, module_exit, MODULE_LICENSE, MODULE_AUTHOR, MODULE_DESCRIPTION
#include <linux/fs.h> // for file operations: open, release, read, write
#include <linux/cdev.h> // cdev_init, cdev_add, cdev_del
#include <linux/uaccess.h> // copy_from_user, copy_to_user

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
