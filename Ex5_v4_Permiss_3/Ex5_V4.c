#include <linux/module.h> // module_init, module_exit, MODULE_LICENSE, MODULE_AUTHOR, MODULE_DESCRIPTION
#include <linux/fs.h> // file operations: open, release, read, write
#include <linux/cdev.h> // cdev_init, cdev_add, cdev_del
#include <linux/device.h> // class_create, device_create, device_destroy, class_destroy
#include <linux/uaccess.h> // copy_from_user, copy_to_user
#include <linux/init.h> // __init, __exit

#define DEVICE_COUNT 5
#define BUF_SIZE 1024
#define DEVICE_NAME "Ex5_V4_"

// Permission flags
#define RDONLY  0x01
#define WRONLY  0x10
#define RDWR    0x11

// Device structure
struct my_device {
    char name[20];
    int permission;
    char buffer[BUF_SIZE];
    struct cdev cdev;
};

// Device array
static struct my_device devices[DEVICE_COUNT] = 
{
    { .name = "Ex5_V4_0", .permission = RDWR },
    { .name = "Ex5_V4_1", .permission = RDONLY },
    { .name = "Ex5_V4_2", .permission = WRONLY },
    { .name = "Ex5_V4_3", .permission = RDWR },
    { .name = "Ex5_V4_4", .permission = WRONLY },
};

static dev_t base_dev; // Base device number
static struct class *my_class; // Device class

// Function to check permissions
static int check_permission(int permission, int access_mode) 
{
    if (permission == RDWR) return 0;
    if ((permission == RDONLY) && (access_mode & FMODE_WRITE)) return -EPERM;
    if ((permission == WRONLY) && (access_mode & FMODE_READ)) return -EPERM;
    return 0;
}

// File operations
static int my_open(struct inode *inode, struct file *filp) 
{
    int minor = iminor(inode);
    struct my_device *dev = &devices[minor];

    if (check_permission(dev->permission, filp->f_mode)) 
    {
        pr_info("%s: permission denied\n", dev->name);
        return -EPERM;
    }

    filp->private_data = dev;
    pr_info("%s: device opened\n", dev->name);
    return 0;
}

// Function to release the device
static int my_release(struct inode *inode, struct file *filp) 
{
    struct my_device *dev = filp->private_data;
    pr_info("%s: device closed\n", dev->name);
    return 0;
}

// Function to read from the device
static ssize_t my_read(struct file *filp, char __user *buf, size_t count, loff_t *offset) 
{
    struct my_device *dev = filp->private_data;
    if (copy_to_user(buf, dev->buffer + *offset, count))
        return -EFAULT;
    pr_info("%s: read %zu bytes\n", dev->name, count);
    return count;
}

// Function to write to the device
static ssize_t my_write(struct file *filp, const char __user *buf, size_t count, loff_t *offset) 
{
    struct my_device *dev = filp->private_data;
    if (copy_from_user(dev->buffer + *offset, buf, count))
        return -EFAULT;
    pr_info("%s: wrote %zu bytes\n", dev->name, count);
    return count;
}

// File operations structure
static struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = my_open,
    .release = my_release,
    .read = my_read,
    .write = my_write,
};

// Module initialization and cleanup functions
static int __init my_init(void) 
{
    int i, ret;
    // Allocate a range of device numbers
    ret = alloc_chrdev_region(&base_dev, 0, DEVICE_COUNT, DEVICE_NAME);
    if (ret < 0) return ret;
    // Create a class for the devices
    my_class = class_create("Ex5_V4_class");
    if (IS_ERR(my_class)) 
    {
        unregister_chrdev_region(base_dev, DEVICE_COUNT);
        return PTR_ERR(my_class);
    }
    // Initialize and add each device
    for (i = 0; i < DEVICE_COUNT; i++) 
    {
        cdev_init(&devices[i].cdev, &fops); // Initialize the character device
        devices[i].cdev.owner = THIS_MODULE; // Set the owner of the cdev

        ret = cdev_add(&devices[i].cdev, MKDEV(MAJOR(base_dev), i), 1); // Add the character device
        if (ret) 
        {
            pr_err("Failed to add cdev for %s\n", devices[i].name);
            goto fail;
        }
        
        device_create(my_class, NULL, MKDEV(MAJOR(base_dev), i), NULL, "%s", devices[i].name); // Create a device node in /dev
    }

    pr_info("Driver loaded successfully\n");
    return 0;

// Cleanup in case of failure
fail:
    while (--i >= 0) 
    {
        device_destroy(my_class, MKDEV(MAJOR(base_dev), i)); // Destroy the device node
        cdev_del(&devices[i].cdev); // Delete the character device
    }
    class_destroy(my_class); // Destroy the class
    unregister_chrdev_region(base_dev, DEVICE_COUNT); // Unregister the device numbers
    return ret;
}

// Module exit function
static void __exit my_exit(void) 
{
    int i;
    // Destroy each device and clean up
    for (i = 0; i < DEVICE_COUNT; i++) 
    {
        device_destroy(my_class, MKDEV(MAJOR(base_dev), i)); // Destroy the device node
        cdev_del(&devices[i].cdev); // Delete the character device
    }
    class_destroy(my_class); // Destroy the class
    unregister_chrdev_region(base_dev, DEVICE_COUNT); // Unregister the device numbers
    pr_info("Driver unloaded\n");
}

module_init(my_init);
module_exit(my_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Leo");
MODULE_DESCRIPTION("Character devices with struct-based definition");
