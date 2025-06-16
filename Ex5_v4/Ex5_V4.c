#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <linux/init.h>

#define DEVICE_COUNT 5
#define BUF_SIZE 1024
#define DEVICE_NAME "Ex5_V4_"

// Permission flags
#define RDONLY  0x01
#define WRONLY  0x10
#define RDWR    0x11

struct my_device {
    char name[20];
    int permission;
    char buffer[BUF_SIZE];
    struct cdev cdev;
};

static struct my_device devices[DEVICE_COUNT] = {
    { .name = "Ex5_V4_0", .permission = RDWR },
    { .name = "Ex5_V4_1", .permission = RDONLY },
    { .name = "Ex5_V4_2", .permission = WRONLY },
    { .name = "Ex5_V4_3", .permission = RDWR },
    { .name = "Ex5_V4_4", .permission = WRONLY },
};

static dev_t base_dev;
static struct class *my_class;

static int check_permission(int permission, int access_mode) {
    if (permission == RDWR) return 0;
    if ((permission == RDONLY) && (access_mode & FMODE_WRITE)) return -EPERM;
    if ((permission == WRONLY) && (access_mode & FMODE_READ)) return -EPERM;
    return 0;
}

static int my_open(struct inode *inode, struct file *filp) {
    int minor = iminor(inode);
    struct my_device *dev = &devices[minor];

    if (check_permission(dev->permission, filp->f_mode)) {
        pr_info("%s: permission denied\n", dev->name);
        return -EPERM;
    }

    filp->private_data = dev;
    pr_info("%s: device opened\n", dev->name);
    return 0;
}

static int my_release(struct inode *inode, struct file *filp) {
    struct my_device *dev = filp->private_data;
    pr_info("%s: device closed\n", dev->name);
    return 0;
}

static ssize_t my_read(struct file *filp, char __user *buf, size_t count, loff_t *offset) {
    struct my_device *dev = filp->private_data;
    if (copy_to_user(buf, dev->buffer + *offset, count))
        return -EFAULT;
    pr_info("%s: read %zu bytes\n", dev->name, count);
    return count;
}

static ssize_t my_write(struct file *filp, const char __user *buf, size_t count, loff_t *offset) {
    struct my_device *dev = filp->private_data;
    if (copy_from_user(dev->buffer + *offset, buf, count))
        return -EFAULT;
    pr_info("%s: wrote %zu bytes\n", dev->name, count);
    return count;
}

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = my_open,
    .release = my_release,
    .read = my_read,
    .write = my_write,
};

static int __init my_init(void) {
    int i, ret;

    ret = alloc_chrdev_region(&base_dev, 0, DEVICE_COUNT, DEVICE_NAME);
    if (ret < 0) return ret;

    my_class = class_create("Ex5_V4_class");
    if (IS_ERR(my_class)) {
        unregister_chrdev_region(base_dev, DEVICE_COUNT);
        return PTR_ERR(my_class);
    }

    for (i = 0; i < DEVICE_COUNT; i++) {
        cdev_init(&devices[i].cdev, &fops);
        devices[i].cdev.owner = THIS_MODULE;

        ret = cdev_add(&devices[i].cdev, MKDEV(MAJOR(base_dev), i), 1);
        if (ret) {
            pr_err("Failed to add cdev for %s\n", devices[i].name);
            goto fail;
        }

        device_create(my_class, NULL, MKDEV(MAJOR(base_dev), i), NULL, "%s", devices[i].name);
    }

    pr_info("Driver loaded successfully\n");
    return 0;

fail:
    while (--i >= 0) {
        device_destroy(my_class, MKDEV(MAJOR(base_dev), i));
        cdev_del(&devices[i].cdev);
    }
    class_destroy(my_class);
    unregister_chrdev_region(base_dev, DEVICE_COUNT);
    return ret;
}

static void __exit my_exit(void) {
    int i;
    for (i = 0; i < DEVICE_COUNT; i++) {
        device_destroy(my_class, MKDEV(MAJOR(base_dev), i));
        cdev_del(&devices[i].cdev);
    }
    class_destroy(my_class);
    unregister_chrdev_region(base_dev, DEVICE_COUNT);
    pr_info("Driver unloaded\n");
}

module_init(my_init);
module_exit(my_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Leo");
MODULE_DESCRIPTION("Character devices with struct-based definition");
