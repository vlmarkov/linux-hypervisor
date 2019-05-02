
// https://stackoverflow.com/questions/21422433/how-does-the-linux-kernel-get-info-about-the-processors-and-the-cores
// https://opensourceforu.com/2011/08/io-control-in-linux/

#include <linux/fs.h>
#include <linux/init.h>
#include <linux/cdev.h>
#include <linux/module.h>
#include <linux/kernel.h>

#include "../include/private/ioctl.h"
#include "../include/private/io.h"

#include "../include/ioctl.h"

static dev_t hypervisor2_dev = 0;
static struct class *hypervisor2_class = NULL;
static struct cdev hypervisor2_cdev;

static struct file_operations hypervisor2_fops =
{
    .owner          = THIS_MODULE,
    .read           = hypervisor2_read,
    .write          = hypervisor2_write,
    .open           = hypervisor2_open,
    .unlocked_ioctl = hypervisor2_ioctl,
    .release        = hypervisor2_release,
};

// Initialize the module - Register the character device 
static int __init hypervisor2_init(void)
{
    int ret = 0;
    struct device *dev_ret = NULL;
  
    if ((ret = alloc_chrdev_region(&hypervisor2_dev, DEVICE_MINOR, DEVICE_MINOR_CNT, DEVICE_NAME)) < 0)
    {
        return ret;
    }
 
    cdev_init(&hypervisor2_cdev, &hypervisor2_fops);
 
    if ((ret = cdev_add(&hypervisor2_cdev, hypervisor2_dev, DEVICE_MINOR_CNT)) < 0)
    {
        return ret;
    }
     
    if (IS_ERR(hypervisor2_class = class_create(THIS_MODULE, "char")))
    {
        cdev_del(&hypervisor2_cdev);
        unregister_chrdev_region(hypervisor2_dev, DEVICE_MINOR_CNT);
        return PTR_ERR(hypervisor2_class);
    }

    if (IS_ERR(dev_ret = device_create(hypervisor2_class, NULL, hypervisor2_dev, NULL, DEVICE_NAME)))
    {
        class_destroy(hypervisor2_class);
        cdev_del(&hypervisor2_cdev);
        unregister_chrdev_region(hypervisor2_dev, DEVICE_MINOR_CNT);
        return PTR_ERR(dev_ret);
    }

    printk(KERN_INFO "[%s] Init!\n", DEVICE_NAME);
    return 0;
}

// Exit - unregister the appropriate file from /proc 
static void __exit hypervisor2_exit(void)
{
    device_destroy(hypervisor2_class, hypervisor2_dev);
    class_destroy(hypervisor2_class);
    cdev_del(&hypervisor2_cdev);
    unregister_chrdev_region(hypervisor2_dev, DEVICE_MINOR_CNT);

    printk(KERN_INFO "[%s] Exit!\n", DEVICE_NAME);
    return;
}

module_init(hypervisor2_init);
module_exit(hypervisor2_exit);
 
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Markov V.A.");
MODULE_DESCRIPTION("A simple hypervisor2 module");
