#include <linux/uaccess.h>

#include "../include/ioctl.h"

int hypervisor2_open(struct inode *inode, struct file *file)
{
    printk(KERN_INFO "[%s] Module open!\n", DEVICE_NAME);
    return 0;
}

int hypervisor2_release(struct inode *inode, struct file *file)
{
    printk(KERN_INFO "[%s] Module release!\n", DEVICE_NAME);
    return 0;
}

ssize_t hypervisor2_read(struct file *filp, char __user *buf, size_t len, loff_t *off)
{
    printk(KERN_INFO "[%s] Module read!\n", DEVICE_NAME);
    return 0;
}

ssize_t hypervisor2_write(struct file *filp, const char __user *buf, size_t len, loff_t *off)
{
    printk(KERN_INFO "[%s] Module write!\n", DEVICE_NAME);
    return 0;
}
