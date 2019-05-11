#include <asm/virtext.h>
#include <linux/slab.h>
#include <linux/tboot.h>

#include "../include/private/bit.h"
#include "../include/private/vmm.h"
#include "../include/private/cpu.h"
#include "../include/ioctl.h"

static u32 hypervisor2_get_vmcs_revision_identifier(void)
{
    return 0x8fffffff & __rdmsr(IA32_VMX_BASIC);
}

void hypervisor2_vmxon_setup_revision_id(vcpu_t *vcpu)
{
    ((u32 *)vcpu->vmxon_region.p)[0] = hypervisor2_get_vmcs_revision_identifier();
}

void hypervisor2_vmcs_setup_revision_id(vcpu_t *vcpu)
{
    ((u32 *)vcpu->vmcs.p)[0] = hypervisor2_get_vmcs_revision_identifier();
}

void hypervisor2_vmx_cache_capabilities_set(vcpu_t *vcpu)
{
    vcpu->ia32_vmx_basic   = __rdmsr(IA32_VMX_BASIC);
    vcpu->vmcs_revision_id = vcpu->ia32_vmx_basic & 0x8fffffff;
    vcpu->has_true_ctls    = !!(vcpu->ia32_vmx_basic & __BIT(55));

    vcpu->ia32_vmx_pinbased_ctls  = __rdmsr(IA32_VMX_PINBASED_CTLS);
    vcpu->ia32_vmx_procbased_ctls = __rdmsr(IA32_VMX_PROCBASED_CTLS);
    vcpu->ia32_vmx_exit_ctls      = __rdmsr(IA32_VMX_EXIT_CTLS);
    vcpu->ia32_vmx_entry_ctls     = __rdmsr(IA32_VMX_ENTRY_CTLS);

    if (vcpu->has_true_ctls)
    {
        vcpu->ia32_vmx_true_pinbased_ctls =
            __rdmsr(IA32_VMX_TRUE_PINBASED_CTLS);
        vcpu->ia32_vmx_true_procbased_ctls =
            __rdmsr(IA32_VMX_PROCBASED_CTLS);
        vcpu->ia32_vmx_true_exit_ctls =
            __rdmsr(IA32_VMX_TRUE_EXIT_CTLS);
        vcpu->ia32_vmx_true_entry_ctls =
            __rdmsr(IA32_VMX_TRUE_ENTRY_CTLS);
    }

    vcpu->pinbased_allowed0 = 0xffffffff &
            (vcpu->has_true_ctls ? vcpu->ia32_vmx_true_pinbased_ctls
                                 : vcpu->ia32_vmx_pinbased_ctls);
    vcpu->pinbased_allowed1 =
            (vcpu->has_true_ctls ? vcpu->ia32_vmx_true_pinbased_ctls
                                 : vcpu->ia32_vmx_pinbased_ctls) >> 32;

    vcpu->procbased_allowed0 = 0xffffffff &
            (vcpu->has_true_ctls ? vcpu->ia32_vmx_true_procbased_ctls
                                 : vcpu->ia32_vmx_procbased_ctls);
    vcpu->procbased_allowed1 =
            (vcpu->has_true_ctls ? vcpu->ia32_vmx_true_procbased_ctls
                                 : vcpu->ia32_vmx_procbased_ctls) >> 32;

    vcpu->exit_ctls_allowed0 = 0xffffffff &
            (vcpu->has_true_ctls ? vcpu->ia32_vmx_true_exit_ctls
                                 : vcpu->ia32_vmx_exit_ctls);
    vcpu->exit_ctls_allowed1 =
            (vcpu->has_true_ctls ? vcpu->ia32_vmx_true_exit_ctls
                                 : vcpu->ia32_vmx_exit_ctls) >> 32;

    vcpu->entry_ctls_allowed0 = 0xffffffff &
            (vcpu->has_true_ctls ? vcpu->ia32_vmx_true_entry_ctls
                                 : vcpu->ia32_vmx_entry_ctls);
    vcpu->entry_ctls_allowed1 =
            (vcpu->has_true_ctls ? vcpu->ia32_vmx_true_entry_ctls
                                 : vcpu->ia32_vmx_entry_ctls) >> 32;
}

int hypervisor2_vmx_init(void)
{
    if (hypervisor2_cpu_vmx_support())
    {
        printk(KERN_INFO "[%s] VMX is not supported in this machine!\n", DEVICE_NAME);
        return -1;
    }

    if (!hypervisor2_cpu_vmx_bios_support())
    {
        printk(KERN_INFO "[%s] VMX is not enabled in bios!\n", DEVICE_NAME);
        return -1;       
    }

    printk(KERN_INFO "[%s] Initializing VMX operations...\n", DEVICE_NAME);

    if (hypervisor2_vmm_init() != 0)
        return -EFAULT;

    on_each_cpu(hypervisor2_vmm_start, NULL, 1);

    return 0;
}

int hypervisor2_vmx_free(void)
{
    printk(KERN_INFO "[%s] Terminating VMM...\n", DEVICE_NAME);

    on_each_cpu(hypervisor2_vmm_stop, NULL, 1);

    hypervisor2_vmm_free();

    return 0;
}
