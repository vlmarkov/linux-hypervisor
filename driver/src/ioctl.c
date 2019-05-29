#include <linux/uaccess.h>

#include "../include/private/cpu.h"
#include "../include/ioctl.h"

int ioctl_hypervisor_id_get(unsigned long arg)
{
    int ret = 0;
    void __user *user_ptr = (void __user *)arg;
    hypervisor_id_t hypervisor_id;
    struct cpuinfo_x86 cpuinfo;

    memset(&cpuinfo, 0, sizeof(struct cpuinfo_x86));  
    memset(&hypervisor_id, 0, sizeof(hypervisor_id_t));

    cpu_id_get(&cpuinfo);

    hypervisor_id.ncores = cpuinfo.booted_cores;
    hypervisor_id.vmx = cpu_has(&cpuinfo, X86_FEATURE_VMX) ? 1 : 0;

    if (cpuinfo.x86_vendor_id[0])
        memcpy(&hypervisor_id.vendor_id, &cpuinfo.x86_vendor_id, 16);

    if (copy_to_user(user_ptr, &hypervisor_id, sizeof(hypervisor_id_t)))
    {
        pr_warn(KERN_INFO "[%s] Unable to copy to userspace!\n", DEVICE_NAME);
        ret = -EFAULT;
    }

    return ret;
}
