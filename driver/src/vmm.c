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

    cpu_id_get(&cpu_info);

    vmm.size = cpu_info.booted_cores;
    vmm.vcpu = kzalloc(sizeof(vcpu_t) * vmm.size, GFP_KERNEL);
    if (vmm.vcpu == NULL)
    {
        printk(KERN_INFO "[%s] Can't allocate memory for VMM\n", DEVICE_NAME);
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
    const int cpu = raw_smp_processor_id();
    vcpu_t *vcpu = &vmm.vcpu[cpu];
    if (IS_ERR(vcpu))
    {
        printk(KERN_INFO "[%s] VCPU is corrupted! %d logical processor\n",
               DEVICE_NAME, cpu);
        return;
    }

    // Sets MSR IA32_VMX_BASIC
    vmx_set_cache_capabilities(vcpu);

    /*
     * Before executing VMXON, software should allocate a naturally aligned
     * 4-KByte region of memory (VMXON and VMCS) that a logical processor
     * may use to support VMX operation.
     */
    if (vmpage_init(vcpu) <= 0)
        return;

    // Sets VMCS revision identifier to VMXON and VMCS regions
    vmx_set_vmxon_revision_id(vcpu);
    vmx_set_vmcs_revision_id(vcpu);

    if (cpu_vmx_on(vcpu) != 0)
        return;

    printk(KERN_INFO
           "[%s] VMX operation enabled successfully on %d logical processor\n",
           DEVICE_NAME, cpu);

    return;
}

void vmm_stop(void *unused)
{
    const int cpu = raw_smp_processor_id();
    vcpu_t *vcpu = &vmm.vcpu[cpu];

    cpu_vm_clear(vcpu);

    cpu_vmx_off(vcpu);

    vmpage_free(vcpu, 5);

    vcpu->may_free = true;

    printk(KERN_INFO
           "[%s] VMX operation disabled successfully on %d logical processor\n",
           DEVICE_NAME, cpu);
}
