#include <linux/fs.h>
#include <linux/cdev.h>

#include "../include/private/ioctl.h"

#include "../include/ioctl.h"

static dev_t vmm_dev = 0;
static struct class *vmm_class = NULL;
static struct cdev vmm_cdev;

static int vmm_device_open(struct inode *inode, struct file *file);
static int vmm_device_release(struct inode *inode, struct file *file);
static ssize_t vmm_device_read(struct file *filp, char __user *buf, size_t len, loff_t *off);
static ssize_t vmm_device_write(struct file *filp, const char __user *buf, size_t len, loff_t *off);
static long vmm_device_ioctl(struct file *file, unsigned int cmd, unsigned long arg);

static struct file_operations vmm_fops =
{
    .owner          = THIS_MODULE,
    .read           = vmm_device_read,
    .write          = vmm_device_write,
    .open           = vmm_device_open,
    .unlocked_ioctl = vmm_device_ioctl,
    .release        = vmm_device_release,
};

static int vmm_device_open(
    struct inode *inode,
    struct file *file)
{
    printk(KERN_INFO "[%s] Open!\n", MODULE_NAME);
    return 0;
}

static int vmm_device_release(
    struct inode *inode,
    struct file *file)
{
    printk(KERN_INFO "[%s] Release!\n", MODULE_NAME);
    return 0;
}

static ssize_t vmm_device_read(
    struct file *filp,
    char __user *buf,
    size_t len,
    loff_t *off)
{
    printk(KERN_INFO "[%s] Read!\n", MODULE_NAME);
    return 0;
}

static ssize_t vmm_device_write(
    struct file *filp,
    const char __user *buf,
    size_t len,
    loff_t *off)
{
    printk(KERN_INFO "[%s] Write!\n", MODULE_NAME);
    return 0;
}

static long vmm_device_ioctl(
    struct file *file,
    unsigned int cmd,
    unsigned long arg)
{
    int ret = 0;

    switch (cmd)
    {
        case VMM_IOCTL_GET_CPU_ID:
            ret = vmm_ioctl_cpu_id_get(arg);
            break;

        default:
            ret = -ENOTTY;
    }

    return ret;
}

int vmm_device_init(void)
{
    int ret = 0;
    struct device *dev_ret = NULL;
  
    if ((ret = alloc_chrdev_region(
        &vmm_dev, DEVICE_MINOR, DEVICE_MINOR_CNT, MODULE_NAME)) < 0)
    {
        return ret;
    }
 
    cdev_init(&vmm_cdev, &vmm_fops);
 
    if ((ret = cdev_add(
        &vmm_cdev, vmm_dev, DEVICE_MINOR_CNT)) < 0)
    {
        return ret;
    }
     
    if (IS_ERR(vmm_class = class_create(THIS_MODULE, "char")))
    {
        cdev_del(&vmm_cdev);
        unregister_chrdev_region(vmm_dev, DEVICE_MINOR_CNT);
        return PTR_ERR(vmm_class);
    }

    if (IS_ERR(dev_ret = device_create(vmm_class, NULL, vmm_dev, NULL, MODULE_NAME)))
    {
        class_destroy(vmm_class);
        cdev_del(&vmm_cdev);
        unregister_chrdev_region(vmm_dev, DEVICE_MINOR_CNT);
        return PTR_ERR(dev_ret);
    }

    return 0;
}

int vmm_device_free(void)
{
    device_destroy(vmm_class, vmm_dev);
    class_destroy(vmm_class);
    cdev_del(&vmm_cdev);
    unregister_chrdev_region(vmm_dev, DEVICE_MINOR_CNT);
    return 0;
}
