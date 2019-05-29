#include <linux/fs.h>
#include <linux/cdev.h>

#include "../include/private/ioctl.h"

#include "../include/ioctl.h"

static dev_t dev = 0;
static struct class *class = NULL;
static struct cdev cdev;

static int device_open(struct inode *inode, struct file *file);
static int device_release(struct inode *inode, struct file *file);
static ssize_t device_read(struct file *filp, char __user *buf, size_t len, loff_t *off);
static ssize_t device_write(struct file *filp, const char __user *buf, size_t len, loff_t *off);
static long device_ioctl(struct file *file, unsigned int cmd, unsigned long arg);

static struct file_operations device_fops =
{
    .owner          = THIS_MODULE,
    .read           = device_read,
    .write          = device_write,
    .open           = device_open,
    .unlocked_ioctl = device_ioctl,
    .release        = device_release,
};

static int device_open(
    struct inode *inode,
    struct file *file)
{
    printk(KERN_INFO "[%s] Open!\n", DEVICE_NAME);
    return 0;
}

static int device_release(
    struct inode *inode,
    struct file *file)
{
    printk(KERN_INFO "[%s] Release!\n", DEVICE_NAME);
    return 0;
}

static ssize_t device_read(
    struct file *filp,
    char __user *buf,
    size_t len,
    loff_t *off)
{
    printk(KERN_INFO "[%s] Read!\n", DEVICE_NAME);
    return 0;
}

static ssize_t device_write(
    struct file *filp,
    const char __user *buf,
    size_t len,
    loff_t *off)
{
    printk(KERN_INFO "[%s] Write!\n", DEVICE_NAME);
    return 0;
}

static long device_ioctl(
    struct file *file,
    unsigned int cmd,
    unsigned long arg)
{
    int ret = 0;

    switch (cmd)
    {
        case HYPERVISOR_IOCTL_GET_ID:
            ret = ioctl_hypervisor_id_get(arg);
            break;

        default:
            ret = -ENOTTY;
    }

    return ret;
}

int device_init(void)
{
    int ret = 0;
    struct device *dev_ret = NULL;

    // Allocates a range of char device numbers
    ret = alloc_chrdev_region(&dev, DEVICE_MINOR, DEVICE_MINOR_CNT, DEVICE_NAME);
    if (ret < 0)
    {
        printk(KERN_INFO "[%s] Chardev allocation is failed!\n", DEVICE_NAME);
        goto exit;
    }

    printk(KERN_INFO "[%s] Chardev major number is %d\n", DEVICE_NAME, MAJOR(dev));
 
    // Initializes a cdev structure
    cdev_init(&cdev, &device_fops);
 
    // Adds a char device to the system
    ret = cdev_add(&cdev, dev, DEVICE_MINOR_CNT);
    if (ret < 0)
    {
        printk(KERN_INFO "[%s] Unable to add cdev!\n", DEVICE_NAME);
        goto unregister;
    }

    // Creates a device class
    class = class_create(THIS_MODULE, DEVICE_NAME);
    if (IS_ERR(class))
    {
        printk(KERN_INFO "[%s] Unable to create class!\n", DEVICE_NAME);
        ret = PTR_ERR(class);
        goto delete;
    }

    // Creates a device and registers it with sysfs
    dev_ret = device_create(class, NULL, dev, NULL, DEVICE_NAME);
    if (IS_ERR(dev_ret))
    {
        printk(KERN_INFO "[%s] Unable to create device!\n", DEVICE_NAME);
        ret = PTR_ERR(dev_ret);
        goto destroy;
    }

    return 0; // Root exit point

destroy:
    class_destroy(class);
delete:
    cdev_del(&cdev);
unregister:
    unregister_chrdev_region(dev, DEVICE_MINOR_CNT);
exit:
    return ret;
}

int device_free(void)
{
    // Unregisters and cleans up device
    device_destroy(class, dev);

    // Destroys a struct class structure
    class_destroy(class);

    // Removes a char device to the system
    cdev_del(&cdev);

    // Returns a range of device numbers
    unregister_chrdev_region(dev, DEVICE_MINOR_CNT);
    return 0;
}
