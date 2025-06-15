#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/uaccess.h>

#define DEVICE_NAME "mychardev"
#define BUF_SIZE 1024

static dev_t dev_number;
static struct cdev my_cdev;
static struct class *my_class;
static struct device *my_device;

static char kernel_buffer[BUF_SIZE];
static int buffer_len = 0;

static int my_open(struct inode *inode, struct file *file)
{
    printk(KERN_INFO "mychardev: Device opened\n");
    return 0;
}

static int my_release(struct inode *inode, struct file *file)
{
    printk(KERN_INFO "mychardev: Device closed\n");
    return 0;
}

static ssize_t my_write(struct file *file, const char __user *buf, size_t len, loff_t *offset)
{
    if (len > BUF_SIZE) len = BUF_SIZE;
    if (copy_from_user(kernel_buffer, buf, len) != 0)
        return -EFAULT;
    buffer_len = len;
    printk(KERN_INFO "mychardev: Received %d bytes\n", buffer_len);
    return len;
}

static ssize_t my_read(struct file *file, char __user *buf, size_t len, loff_t *offset)
{
    if (*offset >= buffer_len) return 0;
    if (len > buffer_len - *offset) len = buffer_len - *offset;

    if (copy_to_user(buf, kernel_buffer + *offset, len) != 0)
        return -EFAULT;

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
    int ret;
    // Allocate a device number
    ret = alloc_chrdev_region(&dev_number, 0, 1, DEVICE_NAME);
    if (ret < 0) return ret;
    // Initialize the character device
    cdev_init(&my_cdev, &fops);
    my_cdev.owner = THIS_MODULE;
    // Add the character device
    ret = cdev_add(&my_cdev, dev_number, 1);
    if (ret < 0) {
        unregister_chrdev_region(dev_number, 1);
        return ret;
    }
    // Create a class
    my_class = class_create("my_class");
    if (IS_ERR(my_class)) {
        cdev_del(&my_cdev);
        unregister_chrdev_region(dev_number, 1);
        return PTR_ERR(my_class);
    }
    // Create a device
    my_device = device_create(my_class, NULL, dev_number, NULL, DEVICE_NAME);
    if (IS_ERR(my_device)) {
        class_destroy(my_class);
        cdev_del(&my_cdev);
        unregister_chrdev_region(dev_number, 1);
        return PTR_ERR(my_device);
    }

    printk(KERN_INFO "mychardev: registered, device created at /dev/%s\n", DEVICE_NAME);
    return 0;
}

static void __exit chardev_exit(void)
{
    device_destroy(my_class, dev_number);
    class_destroy(my_class);
    cdev_del(&my_cdev);
    unregister_chrdev_region(dev_number, 1);
    printk(KERN_INFO "mychardev: unregistered\n");
}

module_init(chardev_init);
module_exit(chardev_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Leo");
MODULE_DESCRIPTION("Char Device with read/writemak");
