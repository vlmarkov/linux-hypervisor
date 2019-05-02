#include <linux/uaccess.h>

#include "../include/private/cpu.h"

#include "../include/ioctl.h"

static int hypervisor2_ioctl_cpu_id_get(unsigned long arg)
{
    int ret = 0;
    void __user *_cpuid = (void __user *)arg;
    hypervisor2_cpuid_t cpuid;
    struct cpuinfo_x86 c;

    memset(&c, 0, sizeof(struct cpuinfo_x86));  
    memset(&cpuid, 0, sizeof(hypervisor2_cpuid_t));

    hypervisor2_cpu_id_get(&c);

    cpuid.ncores = c.booted_cores;
    cpuid.vmx = cpu_has(&c, X86_FEATURE_VMX) ? 1 : 0;

    if (c.x86_vendor_id[0])
        memcpy(&cpuid.vendor_id, &c.x86_vendor_id, 16);  

    if (copy_to_user(_cpuid, &cpuid, sizeof(hypervisor2_cpuid_t)))
    {
        pr_warn(KERN_INFO "[%s] Unable to copy to userspace\n", DEVICE_NAME);
        ret = -EFAULT;
    }

    return ret;
}

long hypervisor2_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
    int ret = 0;

    switch (cmd)
    {
        case HYPERVISOR2_IOCTL_GET_CPU_ID:
            ret = hypervisor2_ioctl_cpu_id_get(arg);
            break;

        default:
            ret = -ENOTTY;
    }

    return ret;
}
