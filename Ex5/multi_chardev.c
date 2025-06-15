#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/uaccess.h>

#define DEVICE_NAME "mychardev"
#define DEVICE_COUNT 5
#define BUF_SIZE 1024

struct my_device_data {
    struct cdev cdev;
    char buffer[BUF_SIZE];
    int buffer_len;
};

static dev_t dev_number;
static struct class *my_class;
static struct my_device_data devices[DEVICE_COUNT];
static struct device *my_device_nodes[DEVICE_COUNT];

// Common file operations

static int my_open(struct inode *inode, struct file *file)
{
    int minor = iminor(inode); // Get the minor number
    file->private_data = &devices[minor]; // Set the private data
    printk(KERN_INFO "mychardev%d: Device opened\n", minor);
    return 0;
}

static int my_release(struct inode *inode, struct file *file)
{
    int minor = iminor(inode);
    printk(KERN_INFO "mychardev%d: Device closed\n", minor);
    return 0;
}

static ssize_t my_write(struct file *file, const char __user *buf, size_t len, loff_t *offset)
{
    struct my_device_data *dev = file->private_data;

    if (len > BUF_SIZE) len = BUF_SIZE;

    if (copy_from_user(dev->buffer, buf, len))
        return -EFAULT;

    dev->buffer_len = len;
    printk(KERN_INFO "mychardev: Received %d bytes\n", dev->buffer_len);
    return len;
}

static ssize_t my_read(struct file *file, char __user *buf, size_t len, loff_t *offset)
{
    struct my_device_data *dev = file->private_data;

    if (*offset >= dev->buffer_len) return 0;
    if (len > dev->buffer_len - *offset) len = dev->buffer_len - *offset;

    if (copy_to_user(buf, dev->buffer + *offset, len))
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

static int __init multi_chardev_init(void)
{
    int ret, i;
    // Allocate a device number
    ret = alloc_chrdev_region(&dev_number, 0, DEVICE_COUNT, DEVICE_NAME); 
    if (ret < 0) return ret;
    // Create a class
    my_class = class_create("my_multi_class");
    if (IS_ERR(my_class)) {
        unregister_chrdev_region(dev_number, DEVICE_COUNT);
        return PTR_ERR(my_class);
    }
    // Create devices
    for (i = 0; i < DEVICE_COUNT; i++) {
        cdev_init(&devices[i].cdev, &fops); // Initialize the cdev
        devices[i].cdev.owner = THIS_MODULE; // Set the owner
        devices[i].buffer_len = 0;
    
        ret = cdev_add(&devices[i].cdev, MKDEV(MAJOR(dev_number), i), 1); // Add the character device
        if (ret < 0) 
        {
            printk(KERN_ERR "Failed to add cdev for device %d\n", i);
            while (--i >= 0) 
            {
                device_destroy(my_class, MKDEV(MAJOR(dev_number), i)); // Destroy the device
                cdev_del(&devices[i].cdev); // Delete the character device
            }
            class_destroy(my_class); // Destroy the class
            unregister_chrdev_region(dev_number, DEVICE_COUNT); // Unregister the device
            return ret;
        }

        my_device_nodes[i] = device_create(my_class, NULL, MKDEV(MAJOR(dev_number), i), NULL, "%s%d", DEVICE_NAME, i); // Create the device
        if (IS_ERR(my_device_nodes[i])) 
        {
            printk(KERN_ERR "Failed to create device node for device %d\n", i);
            while (--i >= 0) 
            {
                device_destroy(my_class, MKDEV(MAJOR(dev_number), i)); // Destroy the device
                cdev_del(&devices[i].cdev); // Delete the character device
            }
            class_destroy(my_class); // Destroy the class
            unregister_chrdev_region(dev_number, DEVICE_COUNT); // Unregister the device
            return PTR_ERR(my_device_nodes[i]);
        }
    }

    printk(KERN_INFO "mychardev: Multi-device driver loaded\n");
    return 0;
}

static void __exit multi_chardev_exit(void)
{
    int i;
    for (i = 0; i < DEVICE_COUNT; i++) 
    {
        device_destroy(my_class, MKDEV(MAJOR(dev_number), i)); // Destroy the device
        cdev_del(&devices[i].cdev); // Delete the character device
    }
    class_destroy(my_class); // Destroy the class
    unregister_chrdev_region(dev_number, DEVICE_COUNT); // Unregister the device
    printk(KERN_INFO "mychardev: Multi-device driver unloaded\n");
}

module_init(multi_chardev_init);
module_exit(multi_chardev_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Leo");
MODULE_DESCRIPTION("Character Driver with 5 Devices");
