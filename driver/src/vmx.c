#include <asm/virtext.h>
#include <linux/slab.h>

#include "../include/private/bit.h"
#include "../include/private/vmm.h"
#include "../include/private/cpu.h"
#include "../include/ioctl.h"

// TODO: description
static u32 vmx_get_vmcs_revision_identifier(void)
{
    return 0x8fffffff & __rdmsr(IA32_VMX_BASIC);
}

// TODO: description
int vmx_set_vmxon_revision_id(vcpu_t *vcpu)
{
    if (IS_ERR(vcpu))
        return -EFAULT;

    ((u32 *)vcpu->vmxon_region.p)[0] = vmx_get_vmcs_revision_identifier();

    return 0;
}

// TODO: description
int vmx_set_vmcs_revision_id(vcpu_t *vcpu)
{
    if (IS_ERR(vcpu))
        return -EFAULT;

    ((u32 *)vcpu->vmcs.p)[0] = vmx_get_vmcs_revision_identifier();

    return 0;
}

// TODO: description
int vmx_set_cache_capabilities(vcpu_t *vcpu)
{
    if (IS_ERR(vcpu))
        return -EFAULT;

    vcpu->ia32_vmx_basic   = __rdmsr(IA32_VMX_BASIC);
    vcpu->vmcs_revision_id = vcpu->ia32_vmx_basic & 0x8fffffff;
    vcpu->has_true_ctls    = !!(vcpu->ia32_vmx_basic & __BIT(55));

    vcpu->ia32_vmx_pinbased_ctls  = __rdmsr(IA32_VMX_PINBASED_CTLS);
    vcpu->ia32_vmx_procbased_ctls = __rdmsr(IA32_VMX_PROCBASED_CTLS);
    vcpu->ia32_vmx_exit_ctls      = __rdmsr(IA32_VMX_EXIT_CTLS);
    vcpu->ia32_vmx_entry_ctls     = __rdmsr(IA32_VMX_ENTRY_CTLS);

    if (vcpu->has_true_ctls)
    {
        vcpu->ia32_vmx_true_pinbased_ctls = __rdmsr(IA32_VMX_TRUE_PINBASED_CTLS);
        vcpu->ia32_vmx_true_procbased_ctls = __rdmsr(IA32_VMX_PROCBASED_CTLS);
        vcpu->ia32_vmx_true_exit_ctls = __rdmsr(IA32_VMX_TRUE_EXIT_CTLS);
        vcpu->ia32_vmx_true_entry_ctls = __rdmsr(IA32_VMX_TRUE_ENTRY_CTLS);
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

    return 0;
}
