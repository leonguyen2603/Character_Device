#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/uaccess.h>

#define DEVICE_NAME "mychardev" // Define the device name
#define BUF_SIZE 1024

static int major; // Major number for the device
static char kernel_buffer[BUF_SIZE]; // Kernel buffer to store data
static int buffer_len = 0; // Length of data in the buffer

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

static ssize_t my_write(struct file *file, const char __user *buf, size_t len, loff_t *offset) // Function to handle writing data to the device
{
    if (len > BUF_SIZE) len = BUF_SIZE;
    if (copy_from_user(kernel_buffer, buf, len) != 0) return -EFAULT; // Copy data from user space to kernel space
    buffer_len = len;
    printk(KERN_INFO "mychardev: Received %d bytes\n", buffer_len);
    return len;
}

static ssize_t my_read(struct file *file, char __user *buf, size_t len, loff_t *offset) 
{
    if (*offset >= buffer_len) return 0;
    if (len > buffer_len - *offset) len = buffer_len - *offset;

    if (copy_to_user(buf, kernel_buffer + *offset, len) != 0) return -EFAULT; // Copy data from kernel space to user space

    *offset += len;
    printk(KERN_INFO "mychardev: Sent %ld bytes\n", len);
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
    major = register_chrdev(0, DEVICE_NAME, &fops); // Register the character device
    printk(KERN_INFO "mychardev registered with major %d\n", major);
    return 0;
}

static void __exit chardev_exit(void) 
{
    unregister_chrdev(major, DEVICE_NAME);
    printk(KERN_INFO "mychardev unregistered\n");
}

module_init(chardev_init);
module_exit(chardev_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Leo");
MODULE_DESCRIPTION("Char Device with read/write - method 1");
