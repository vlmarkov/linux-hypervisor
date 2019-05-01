// https://stackoverflow.com/questions/21422433/how-does-the-linux-kernel-get-info-about-the-processors-and-the-cores
// https://opensourceforu.com/2011/08/io-control-in-linux/

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/cdev.h>

#include <linux/fs.h>
#include <linux/uaccess.h>

#include "hypervisor2.h"

static dev_t hypervisor2_dev = 0;
static struct class *hypervisor2_class;
static struct cdev hypervisor2_cdev;

static int __init hypervisor2_init(void);
static void __exit hypervisor2_exit(void);
static int hypervisor2_open(struct inode *inode, struct file *file);
static int hypervisor2_release(struct inode *inode, struct file *file);
static ssize_t hypervisor2_read(struct file *filp, char __user *buf, size_t len,loff_t * off);
static ssize_t hypervisor2_write(struct file *filp, const char *buf, size_t len, loff_t * off);
static long hypervisor2_ioctl(struct file *file, unsigned int cmd, unsigned long arg);
static void hypervisor2_cpuid_get(struct cpuinfo_x86 *c);
static void hypervisor2_cpuid_get(struct cpuinfo_x86 *c);

static struct file_operations hypervisor2_fops =
{
    .owner          = THIS_MODULE,
    .read           = hypervisor2_read,
    .write          = hypervisor2_write,
    .open           = hypervisor2_open,
    .unlocked_ioctl = hypervisor2_ioctl,
    .release        = hypervisor2_release,
};

static void hypervisor2_cpuid_get(struct cpuinfo_x86 *c)
{
    // Get vendor name
    cpuid(0x00000000,
        (unsigned int *)&c->cpuid_level,
        (unsigned int *)&c->x86_vendor_id[0],
        (unsigned int *)&c->x86_vendor_id[8],
        (unsigned int *)&c->x86_vendor_id[4]);

    // Intel-defined flags: level 0x00000001
    if (c->cpuid_level >= 0x00000001)
    {
        u32 capability, excap, ebx, tfms;
        // Get VMX support
        cpuid(0x00000001, &tfms, &ebx, &excap, &capability);
        c->x86_capability[0] = capability;
        c->x86_capability[4] = excap;
    }
    // AMD dosen't support
}

static int hypervisor2_ioctl_cpuid_get(unsigned long arg)
{
    int ret = 0;
    void __user *_cpuid = (void __user *)arg;
    struct hypervisor2_cpuid cpuid;
    struct cpuinfo_x86 c;

    memset(&c, 0, sizeof(struct cpuinfo_x86));  
    memset(&cpuid, 0, sizeof(struct hypervisor2_cpuid));

    hypervisor2_cpuid_get(&c);

    cpuid.vmx = cpu_has(&c, X86_FEATURE_VMX) ? 1 : 0;

    if (c.x86_vendor_id[0])
        memcpy(&cpuid.vendor_id, &c.x86_vendor_id, 16);

    if (copy_to_user(_cpuid, &cpuid, sizeof(struct hypervisor2_cpuid)))
    {
        pr_warn(KERN_INFO ": unable to copy hypervisor2_cpuid to userspace\n");
        ret = -EFAULT;
        goto end;
    }

    ret = 0;

end:
    return ret;
}

static int hypervisor2_open(struct inode *inode, struct file *file)
{
    printk(KERN_INFO "Device File Opened...!!!\n");
    return 0;
}

static int hypervisor2_release(struct inode *inode, struct file *file)
{
    printk(KERN_INFO "Device File Closed...!!!\n");
    return 0;
}

static ssize_t hypervisor2_read(struct file *filp, char __user *buf, size_t len, loff_t *off)
{
    printk(KERN_INFO "Read Function\n");
    return 0;
}

static ssize_t hypervisor2_write(struct file *filp, const char __user *buf, size_t len, loff_t *off)
{
    printk(KERN_INFO "Write function\n");
    return 0;
}

static long hypervisor2_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
    int ret = 0;

    switch (cmd)
    {
        case HYPERVISOR2_IOCTL_GET_CPU_ID:
            ret = hypervisor2_ioctl_cpuid_get(arg);
            break;

        default:
            ret = -ENOTTY;
    }

    return ret;
}

// Initialize the module - Register the character device 
static int __init hypervisor2_init(void)
{
    int ret;
    struct device *dev_ret;
  
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

    printk(KERN_INFO "[%s] Starting...\n", DEVICE_NAME);
    return 0;
}

// Exit - unregister the appropriate file from /proc 
static void __exit hypervisor2_exit(void)
{
    device_destroy(hypervisor2_class, hypervisor2_dev);
    class_destroy(hypervisor2_class);
    cdev_del(&hypervisor2_cdev);
    unregister_chrdev_region(hypervisor2_dev, DEVICE_MINOR_CNT);

    printk(KERN_INFO "[%s] Ending...\n", DEVICE_NAME);
    return;
}

module_init(hypervisor2_init);
module_exit(hypervisor2_exit);
 
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Markov V.A.");
MODULE_DESCRIPTION("A simple hypervisor2 module");
