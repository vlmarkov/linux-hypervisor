// https://stackoverflow.com/questions/21422433/how-does-the-linux-kernel-get-info-about-the-processors-and-the-cores
// https://opensourceforu.com/2011/08/io-control-in-linux/

#include <linux/init.h>
#include <linux/module.h>

#include "../include/private/cpu.h"
#include "../include/private/vmm.h"
#include "../include/private/device.h"

#include "../include/ioctl.h"

static int __init hypervisor_init(void)
{
    // Creates character device
    if (device_init() != 0)
    {
        printk(KERN_INFO "[%s] Can't init device!\n", DEVICE_NAME);
        return -EFAULT;
    }

    // Checks hw capabilty
    if (cpu_vmx_support())
    {
        printk(KERN_INFO "[%s] VMX is not supported by this machine!\n", DEVICE_NAME);
        return -EFAULT;
    }

    // Checks bios capabilty
    if (!cpu_vmx_bios_support())
    {
        printk(KERN_INFO "[%s] VMX is not enabled in bios!\n", DEVICE_NAME);
        return -EFAULT;
    }

    // Inits virtual machine monitor
    if (vmm_init() != 0)
        return -EFAULT;

    // Runs virtual machine on each logical cpu
    on_each_cpu(vmm_start, NULL, 1);

    printk(KERN_INFO "[%s] Successfully init!\n", DEVICE_NAME);
    return 0;
}

static void __exit hypervisor_exit(void)
{
    on_each_cpu(vmm_stop, NULL, 1);

    vmm_free();

    device_free();

    printk(KERN_INFO "[%s] Successfully exit!\n", DEVICE_NAME);
    return;
}

module_init(hypervisor_init); // A Driver initialization entry point
module_exit(hypervisor_exit); // A driver exit entry point
 
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Markov V.A.");
MODULE_DESCRIPTION("A hypervisor type 2 driver");
