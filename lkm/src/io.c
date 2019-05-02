#include <linux/uaccess.h>

#include "../include/ioctl.h"

int hypervisor2_open(struct inode *inode, struct file *file)
{
    printk(KERN_INFO "[%s] Open!\n", DEVICE_NAME);
    return 0;
}

int hypervisor2_release(struct inode *inode, struct file *file)
{
    printk(KERN_INFO "[%s] Release!\n", DEVICE_NAME);
    return 0;
}

ssize_t hypervisor2_read(struct file *filp, char __user *buf, size_t len, loff_t *off)
{
    printk(KERN_INFO "[%s] Read!\n", DEVICE_NAME);
    return 0;
}

ssize_t hypervisor2_write(struct file *filp, const char __user *buf, size_t len, loff_t *off)
{
    printk(KERN_INFO "[%s] Write!\n", DEVICE_NAME);
    return 0;
}
