#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>

#define DEVICE_NAME "my_character_device" // Define the device name
#define BUF_SIZE 1024

static dev_t dev_number; // Device number
static struct cdev my_cdev; // Character device structure
static char kernel_buffer[BUF_SIZE]; // Kernel buffer to store data
static int buffer_len = 0; // Length of data in the buffer

static int my_open(struct inode *inode, struct file *file) // Function to handle opening the device
{
    printk(KERN_INFO "mychardev: Opened\n");
    return 0;
}

static int my_release(struct inode *inode, struct file *file) // Function to handle releasing the device
{
    printk(KERN_INFO "mychardev: Closed\n");
    return 0;
}

static ssize_t my_write(struct file *file, const char __user *buf, size_t len, loff_t *offset) // Function to handle writing data to the device
{
    if (len > BUF_SIZE) len = BUF_SIZE;
    if (copy_from_user(kernel_buffer, buf, len)) return -EFAULT; // Copy data from user space to kernel space
    buffer_len = len;
    printk(KERN_INFO "mychardev: Wrote %d bytes\n", buffer_len);
    return len;
}

static ssize_t my_read(struct file *file, char __user *buf, size_t len, loff_t *offset) // Function to handle reading data from the device
{
    if (*offset >= buffer_len) return 0; // No more data to read
    if (len > buffer_len - *offset) len = buffer_len - *offset; // Adjust length if it exceeds available data

    if (copy_to_user(buf, kernel_buffer + *offset, len)) return -EFAULT; // Copy data from kernel space to user space

    *offset += len;
    printk(KERN_INFO "mychardev: Read %ld bytes\n", len);
    return len;
}

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = my_open,
    .release = my_release,
    .read = my_read,
    .write = my_write,
};

static int __init chardev_init(void) 
{
    int ret;

    ret = alloc_chrdev_region(&dev_number, 0, 1, DEVICE_NAME); // Allocate a character device number

    cdev_init(&my_cdev, &fops); // Initialize the character device structure
    ret = cdev_add(&my_cdev, dev_number, 1); // Add the character device to the system

    printk(KERN_INFO "mychardev registered - major: %d\n", MAJOR(dev_number));
    return 0;
}

static void __exit chardev_exit(void) 
{
    cdev_del(&my_cdev); // Remove the character device from the system
    unregister_chrdev_region(dev_number, 1); // Unregister the character device number
    printk(KERN_INFO "mychardev unregistered\n"); 
}

module_init(chardev_init);
module_exit(chardev_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Leo");
MODULE_DESCRIPTION("Char Device with read/write - method 2");
