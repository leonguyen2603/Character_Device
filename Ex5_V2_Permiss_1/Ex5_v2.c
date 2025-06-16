#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <linux/kmod.h> // call_usermodehelper

#define DEVICE_NAME "Ex5_character_device"
#define DEVICE_COUNT 5
#define BUF_SIZE 1024

struct my_device {
    struct cdev cdev;
    char buffer[BUF_SIZE];
    int buffer_len;
    char name[32];
    umode_t perm;
};

static dev_t dev_number;
static struct class *my_class;
static struct my_device devices[DEVICE_COUNT];
static struct device *my_device_nodes[DEVICE_COUNT];

// Gọi chmod từ kernel bằng usermode helper
static int chmod_user_space(const char *path, umode_t mode)
{
    char mode_str[8];
    char *argv[] = { "/bin/chmod", mode_str, (char *)path, NULL };
    char *envp[] = { "HOME=/", "PATH=/sbin:/bin:/usr/sbin:/usr/bin", NULL };

    snprintf(mode_str, sizeof(mode_str), "%o", mode); // ví dụ: "644"
    return call_usermodehelper(argv[0], argv, envp, UMH_WAIT_PROC);
}

// File operations

static int my_open(struct inode *inode, struct file *file)
{
    int minor = iminor(inode);
    struct my_device *dev = &devices[minor];
    file->private_data = dev;
    printk(KERN_INFO "%s: Device opened\n", dev->name);
    return 0;
}

static int my_release(struct inode *inode, struct file *file)
{
    struct my_device *dev = file->private_data;
    printk(KERN_INFO "%s: Device closed\n", dev->name);
    return 0;
}

static ssize_t my_write(struct file *file, const char __user *buf, size_t len, loff_t *offset)
{
    struct my_device *dev = file->private_data;

    if (len > BUF_SIZE) len = BUF_SIZE;

    if (copy_from_user(dev->buffer, buf, len))
        return -EFAULT;

    dev->buffer_len = len;
    printk(KERN_INFO "%s: Received %d bytes\n", dev->name, dev->buffer_len);
    return len;
}

static ssize_t my_read(struct file *file, char __user *buf, size_t len, loff_t *offset)
{
    struct my_device *dev = file->private_data;

    if (*offset >= dev->buffer_len)
        return 0;
    if (len > dev->buffer_len - *offset)
        len = dev->buffer_len - *offset;

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

// Module init/exit

static int __init multi_chardev_init(void)
{
    int ret, i;
    ret = alloc_chrdev_region(&dev_number, 0, DEVICE_COUNT, DEVICE_NAME);
    if (ret < 0)
        return ret;

    my_class = class_create("my_multi_class");
    if (IS_ERR(my_class)) 
    {
        unregister_chrdev_region(dev_number, DEVICE_COUNT);
        return PTR_ERR(my_class);
    }

    for (i = 0; i < DEVICE_COUNT; i++) 
    {
        snprintf(devices[i].name, sizeof(devices[i].name), "%s%d", DEVICE_NAME, i);
        devices[i].buffer_len = 0;

        // Set permissions
        devices[i].perm = (i == 0) ? 0666 :
                          (i == 1) ? 0600 :
                          (i == 2) ? 0644 :
                          (i == 3) ? 0620 : 0660;
                          (i == 4) ? 0777 : 

        cdev_init(&devices[i].cdev, &fops);
        devices[i].cdev.owner = THIS_MODULE;

        ret = cdev_add(&devices[i].cdev, MKDEV(MAJOR(dev_number), i), 1);
        if (ret < 0) 
        {
            printk(KERN_ERR "Failed to add cdev for device %d\n", i);
            while (--i >= 0) 
            {
                device_destroy(my_class, MKDEV(MAJOR(dev_number), i));
                cdev_del(&devices[i].cdev);
            }
            class_destroy(my_class);
            unregister_chrdev_region(dev_number, DEVICE_COUNT);
            return ret;
        }

        my_device_nodes[i] = device_create(my_class, NULL, MKDEV(MAJOR(dev_number), i), NULL, "%s%d", DEVICE_NAME, i);
        if (IS_ERR(my_device_nodes[i])) 
        {
            printk(KERN_ERR "Failed to create device node for device %d\n", i);
            while (--i >= 0) 
            {
                device_destroy(my_class, MKDEV(MAJOR(dev_number), i));
                cdev_del(&devices[i].cdev);
            }
            class_destroy(my_class);
            unregister_chrdev_region(dev_number, DEVICE_COUNT);
            return PTR_ERR(my_device_nodes[i]);
        }

        // Gọi chmod từ kernel
        {
            char path[64];
            snprintf(path, sizeof(path), "/dev/%s", devices[i].name);
            ret = chmod_user_space(path, devices[i].perm);
            if (ret != 0)
                printk(KERN_WARNING "Failed to chmod %s to %o\n", path, devices[i].perm);
        }
    }

    printk(KERN_INFO "Ex5_character_device: Multi-device driver loaded\n");
    return 0;
}

static void __exit multi_chardev_exit(void)
{
    int i;
    for (i = 0; i < DEVICE_COUNT; i++) 
    {
        device_destroy(my_class, MKDEV(MAJOR(dev_number), i));
        cdev_del(&devices[i].cdev);
    }
    class_destroy(my_class);
    unregister_chrdev_region(dev_number, DEVICE_COUNT);
    printk(KERN_INFO "Ex5_character_device: Multi-device driver unloaded\n");
}

module_init(multi_chardev_init);
module_exit(multi_chardev_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Leo");
MODULE_DESCRIPTION("Character Driver with 5 Devices, each with individual permissions and names");
