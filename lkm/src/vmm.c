#include <linux/slab.h>

#include "../include/private/vmpage.h"
#include "../include/private/vmm.h"
#include "../include/private/vmx.h"
#include "../include/private/cpu.h"
#include "../include/ioctl.h"

static vmm_t hypervisor2_vmm = { 0 };

int hypervisor2_vmm_init(void)
{
    struct cpuinfo_x86 cpu_info;

    hypervisor2_cpu_id_get(&cpu_info);

    printk(KERN_INFO
        "[%s] Initializing VMX area for %d logical processors...\n",
        DEVICE_NAME, cpu_info.booted_cores);

    hypervisor2_vmm.size = cpu_info.booted_cores;
    hypervisor2_vmm.vcpu = kzalloc(sizeof(vcpu_t) * hypervisor2_vmm.size, GFP_KERNEL);
    if (hypervisor2_vmm.vcpu == NULL)
    {
        printk(KERN_INFO "[%s] Can't allocate memory for VMX area\n", DEVICE_NAME);
        return -ENOMEM;
    }

    return 0;
}

int hypervisor2_vmm_free(void)
{
    int i = 0;
    int cnt = 0;

    while (1)
    {
        for (i = 0; i < hypervisor2_vmm.size; i++)
            if (hypervisor2_vmm.vcpu[i].may_free)
                cnt++;

        if (cnt == hypervisor2_vmm.size)
        {
            kfree(hypervisor2_vmm.vcpu);
            break;
        }
        else
        {
            cnt = 0;
        }
    }

    printk(KERN_INFO "[%s] Terminating VMX AREA...\n", DEVICE_NAME);
    return 0;
}

void hypervisor2_vmm_start(void *unused)
{
    int cnt;
    const int cpu = raw_smp_processor_id();
    vcpu_t *vcpu = &hypervisor2_vmm.vcpu[cpu];
    if (!vcpu)
    {
        printk(KERN_INFO
            "[%s] VCPU is corrupted! %d logical processor\n",
            DEVICE_NAME, cpu);
        return;
    }

    hypervisor2_vmx_cache_capabilities_set(vcpu);

    cnt = hypervisor2_vmpage_allocate(vcpu);
    if (cnt <= 0)
        return;

    hypervisor2_vmxon_setup_revision_id(vcpu);
    hypervisor2_vmcs_setup_revision_id(vcpu);

    if (hypervisor2_cpu_vmxon(vcpu) != 0)
        return;

    printk(KERN_INFO
        "[%s] VMX operation enabled successfully on %d logical processor\n",
        DEVICE_NAME, cpu);

    return;
}

void hypervisor2_vmm_stop(void *unused)
{
    const int cpu = raw_smp_processor_id();
    vcpu_t *vcpu = &hypervisor2_vmm.vcpu[cpu];

    hypervisor2_cpu_vmclear(vcpu);
    hypervisor2_cpu_vmxoff(vcpu);
    hypervisor2_vmpage_free(vcpu, 5);
    vcpu->may_free = true;

    printk(KERN_INFO "[%s] VMX operation disabled successfully on %d logical processor\n",
        DEVICE_NAME, cpu);
}
