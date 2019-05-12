// https://stackoverflow.com/questions/21422433/how-does-the-linux-kernel-get-info-about-the-processors-and-the-cores
// https://opensourceforu.com/2011/08/io-control-in-linux/

#include <linux/init.h>
#include <linux/module.h>

#include "../include/private/vmx.h"
#include "../include/private/device.h"
#include "../include/ioctl.h"


// Initialize the module - Register the character device 
static int __init vmm_module_init(void)
{
    if (vmm_device_init() != 0)
    {
        printk(KERN_INFO "[%s] Can't init device!\n", MODULE_NAME);
        vmm_device_free();
        return -EFAULT;
    }

    if (vmm_vmx_init() != 0)
    {
        printk(KERN_INFO "[%s] Can't inti VMX!\n", MODULE_NAME);
        vmm_device_free();
        return -EFAULT;
    }

    printk(KERN_INFO "[%s] Successfully init!\n", MODULE_NAME);
    return 0;
}

// Exit - unregister the appropriate file from /proc 
static void __exit vmm_module_exit(void)
{
    vmm_vmx_free();
    vmm_device_free();

    printk(KERN_INFO "[%s] Successfully exit!\n", MODULE_NAME);
    return;
}

module_init(vmm_module_init);
module_exit(vmm_module_exit);
 
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Markov V.A.");
MODULE_DESCRIPTION("A simple hypervisor2 module");
