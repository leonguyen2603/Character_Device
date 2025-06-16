#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/uaccess.h>

#define DEVICE_NAME "Ex5_chardev_V3"
#define DEVICE_COUNT 5
#define BUF_SIZE 1024

// Quyền truy cập
#define PERM_WRITE 0x01
#define PERM_READ  0x02
#define PERM_RW    0x03

struct my_device_data {
    struct cdev cdev;
    char buffer[BUF_SIZE];
    int buffer_len;
    char name[20];
    uint8_t permission; 
};

static dev_t dev_number;
static struct class *my_class;
static struct my_device_data devices[DEVICE_COUNT];
static struct device *my_device_nodes[DEVICE_COUNT];

static int my_open(struct inode *inode, struct file *file)
{
    int minor = iminor(inode);
    file->private_data = &devices[minor];
    printk(KERN_INFO "%s: Device opened\n", devices[minor].name);
    return 0;
}

static int my_release(struct inode *inode, struct file *file)
{
    struct my_device_data *dev = file->private_data;
    printk(KERN_INFO "%s: Device closed\n", dev->name);
    return 0;
}

static ssize_t my_write(struct file *file, const char __user *buf, size_t len, loff_t *offset)
{
    struct my_device_data *dev = file->private_data;

    if (!(dev->permission & PERM_WRITE)) 
    {
        printk(KERN_WARNING "%s: Write not permitted\n", dev->name);
        return -EPERM;
    }

    if (len > BUF_SIZE) len = BUF_SIZE;

    if (copy_from_user(dev->buffer, buf, len))
        return -EFAULT;

    dev->buffer_len = len;
    printk(KERN_INFO "%s: Received %d bytes\n", dev->name, dev->buffer_len);
    return len;
}

static ssize_t my_read(struct file *file, char __user *buf, size_t len, loff_t *offset)
{
    struct my_device_data *dev = file->private_data;

    if (!(dev->permission & PERM_READ)) 
    {
        printk(KERN_WARNING "%s: Read not permitted\n", dev->name);
        return -EPERM;
    }

    if (*offset >= dev->buffer_len) return 0;
    if (len > dev->buffer_len - *offset) len = dev->buffer_len - *offset;

    if (copy_to_user(buf, dev->buffer + *offset, len))
        return -EFAULT;

    *offset += len;
    printk(KERN_INFO "%s: Sent %ld bytes\n", dev->name, len);
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

    ret = alloc_chrdev_region(&dev_number, 0, DEVICE_COUNT, DEVICE_NAME);
    if (ret < 0) return ret;

    my_class = class_create("my_multi_class"); // kernel 6.8+
    if (IS_ERR(my_class)) 
    {
        unregister_chrdev_region(dev_number, DEVICE_COUNT);
        return PTR_ERR(my_class);
    }

    for (i = 0; i < DEVICE_COUNT; i++) 
    {
        snprintf(devices[i].name, sizeof(devices[i].name), "%s%d", DEVICE_NAME, i);
        devices[i].buffer_len = 0;

        // Gán quyền khác nhau cho mỗi thiết bị
        switch (i) 
        {
            case 0: devices[i].permission = PERM_WRITE; break;
            case 1: devices[i].permission = PERM_READ; break;
            case 2: devices[i].permission = PERM_RW; break;
            case 3: devices[i].permission = PERM_WRITE; break;
            case 4: devices[i].permission = PERM_RW; break;
            default: devices[i].permission = 0;
        }

        cdev_init(&devices[i].cdev, &fops);
        devices[i].cdev.owner = THIS_MODULE;

        ret = cdev_add(&devices[i].cdev, MKDEV(MAJOR(dev_number), i), 1);
        if (ret < 0) {
            printk(KERN_ERR "Failed to add cdev for device %d\n", i);
            goto fail;
        }

        my_device_nodes[i] = device_create(my_class, NULL, MKDEV(MAJOR(dev_number), i), NULL, devices[i].name);
        if (IS_ERR(my_device_nodes[i])) {
            printk(KERN_ERR "Failed to create device node for device %d\n", i);
            cdev_del(&devices[i].cdev);
            goto fail;
        }
    }

    printk(KERN_INFO "Ex5_chardev_V3: Multi-device driver with permission loaded\n");
    return 0;

fail:
    while (--i >= 0) {
        device_destroy(my_class, MKDEV(MAJOR(dev_number), i));
        cdev_del(&devices[i].cdev);
    }
    class_destroy(my_class);
    unregister_chrdev_region(dev_number, DEVICE_COUNT);
    return ret;
}

static void __exit multi_chardev_exit(void)
{
    int i;
    for (i = 0; i < DEVICE_COUNT; i++) {
        device_destroy(my_class, MKDEV(MAJOR(dev_number), i));
        cdev_del(&devices[i].cdev);
    }
    class_destroy(my_class);
    unregister_chrdev_region(dev_number, DEVICE_COUNT);
    printk(KERN_INFO "Ex5_chardev_V3: Driver unloaded\n");
}

module_init(multi_chardev_init);
module_exit(multi_chardev_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Leo");
MODULE_DESCRIPTION("Character Device Driver with Per-Device Permissions");
