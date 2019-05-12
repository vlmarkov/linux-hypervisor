#include <linux/slab.h>

#include "../include/private/vmpage.h"
#include "../include/private/vmm.h"
#include "../include/private/vmx.h"
#include "../include/private/cpu.h"
#include "../include/ioctl.h"

static vmm_t vmm = { 0 };

int vmm_init(void)
{
    struct cpuinfo_x86 cpu_info;

    vmm_cpu_id_get(&cpu_info);

    vmm.size = cpu_info.booted_cores;
    vmm.vcpu = kzalloc(sizeof(vcpu_t) * vmm.size, GFP_KERNEL);
    if (vmm.vcpu == NULL)
    {
        printk(KERN_INFO "[%s] Can't allocate memory for VMM\n", MODULE_NAME);
        return -ENOMEM;
    }

    return 0;
}

int vmm_free(void)
{
    int i = 0;
    int cnt = 0;

    while (1)
    {
        for (i = 0; i < vmm.size; i++)
            if (vmm.vcpu[i].may_free)
                cnt++;

        if (cnt == vmm.size)
        {
            kfree(vmm.vcpu);
            break;
        }
        else
        {
            cnt = 0;
        }
    }

    return 0;
}

void vmm_start(void *unused)
{
    int cnt;
    const int cpu = raw_smp_processor_id();
    vcpu_t *vcpu = &vmm.vcpu[cpu];
    if (!vcpu)
    {
        printk(KERN_INFO
            "[%s] VCPU is corrupted! %d logical processor\n",
            MODULE_NAME, cpu);
        return;
    }

    vmm_vmx_cache_capabilities_set(vcpu);

    cnt = vmm_vmpage_init(vcpu);
    if (cnt <= 0)
        return;

    vmm_vmxon_setup_revision_id(vcpu);
    vmm_vmcs_setup_revision_id(vcpu);

    if (vmm_cpu_vmxon(vcpu) != 0)
        return;

    printk(KERN_INFO
        "[%s] VMX operation enabled successfully on %d logical processor\n",
        MODULE_NAME, cpu);

    return;
}

void vmm_stop(void *unused)
{
    const int cpu = raw_smp_processor_id();
    vcpu_t *vcpu = &vmm.vcpu[cpu];

    vmm_cpu_vmclear(vcpu);
    vmm_cpu_vmxoff(vcpu);
    vmm_vmpage_free(vcpu, 5);
    vcpu->may_free = true;

    printk(KERN_INFO "[%s] VMX operation disabled successfully on %d logical processor\n",
        MODULE_NAME, cpu);
}
