#include <asm/virtext.h>
#include <asm/vmx.h>
#include <asm/perf_event.h>

#include "../include/private/bit.h"
#include "../include/private/cpu.h"
#include "../include/ioctl.h"

static inline u64 __get_cr4(void)
{
    u64 cr4;

    __asm__ __volatile__(
        "movq %%cr4, %0"
        :"=r"(cr4):);

    return cr4;
}

static inline void __set_cr4(u64 cr4)
{
    __asm__ __volatile__(
        "movq %0, %%cr4"
        ::"r"(cr4));
}

static inline int __vmclear(uintptr_t vmcs_pa)
{
    u64 rflags;
    
    __asm__ __volatile__(
        "vmclear %1;"
        SAVE_RFLAGS(rflags)
        :"m"(vmcs_pa));

    if (rflags & (RFLAGS_CF | RFLAGS_ZF))
        return -1;
    return 0;
}

static inline int __vmxon(uintptr_t vmxon_region_pa)
{
    u64 rflags;
    __asm__ __volatile__(
            "vmxon %1;"
            SAVE_RFLAGS(rflags)
            :"m"(vmxon_region_pa));

    rflags &= RFLAGS_CF | RFLAGS_ZF;
    if (rflags == RFLAGS_CF)
            return -1;  /* VMXON failed */
    if (rflags == (RFLAGS_CF | RFLAGS_ZF))
            return 1;  /* VMX is already on */

    return 0;
}

static inline int __vmxoff(void)
{
    u64 rflags;
    __asm__ __volatile__(
            "vmxoff;"
            SAVE_RFLAGS(rflags));
    if (rflags & (RFLAGS_CF | RFLAGS_ZF))
            return -1;
    return 0;
}

static inline void __cpuid_all(u32 leaf, u32 subleaf, u32 *eax, u32 *ebx, u32 *ecx, u32 *edx)
{
    __asm__ __volatile__(
        "cpuid"
        :"=a"(*eax), "=b"(*ebx), "=c"(*ecx), "=d"(*edx)
        :"a"(leaf), "c"(subleaf));
}

static inline u32 __cpuid_ecx(u32 leaf, u32 subleaf)
{
    u32 eax, ebx, ecx, edx;
    __cpuid_all(leaf, subleaf, &eax, &ebx, &ecx, &edx);
    return ecx;
}

// Find out the number of processor cores on the die
static int hypervisor2_cpu_intel_cores_get(struct cpuinfo_x86 *c)
{
    unsigned int eax, ebx, ecx, edx;

    if (c->cpuid_level < 4)
        return 1;

    // Intel has a non-standard dependency on %ecx for this CPUID level.
    cpuid_count(4, 0, &eax, &ebx, &ecx, &edx);
    if (eax & 0x1f)
        return (eax >> 26) + 1;
    else
        return 1;
}

void hypervisor2_cpu_id_get(struct cpuinfo_x86 *cpu_info)
{
    // Get vendor name
    cpuid(0x00000000,
        (unsigned int *)&cpu_info->cpuid_level,
        (unsigned int *)&cpu_info->x86_vendor_id[0],
        (unsigned int *)&cpu_info->x86_vendor_id[8],
        (unsigned int *)&cpu_info->x86_vendor_id[4]);

    // Intel-defined flags: level 0x00000001
    if (cpu_info->cpuid_level >= 0x00000001)
    {
        u32 capability, excap, ebx, tfms;
        // Get VMX support
        cpuid(0x00000001, &tfms, &ebx, &excap, &capability);
        cpu_info->x86_capability[0] = capability;
        cpu_info->x86_capability[4] = excap;
    }

    cpu_info->booted_cores = hypervisor2_cpu_intel_cores_get(cpu_info);
    // AMD dosen't support
}

bool hypervisor2_cpu_vmx_support(void)
{
    u32 ecx = __cpuid_ecx(1, 0);
    return !(ecx & CPUID_1_ECX_VMX);
}

bool hypervisor2_cpu_vmx_bios_support(void)
{
    u64 feature_control;

    feature_control = __rdmsr(IA32_FEATURE_CONTROL);
    if (!(feature_control & FEATURE_CONTROL_LOCK_BIT) ||
        !(feature_control & FEATURE_CONTROL_VMX_OUTSIDE_SMX_ENABLE_BIT))
    {
        printk(KERN_INFO "VMX is not enabled in BIOS\n");
        return false;
    }

    return true;
}

int hypervisor2_cpu_vmxon(vcpu_t *vcpu)
{
    int ret;
    u64 old_cr4 = __get_cr4();
    vcpu->old_vmxe = old_cr4 & CR4_VMXE;

    /* Set CR4.VMXE if necessary */
    if (!vcpu->old_vmxe)
        __set_cr4(old_cr4 | CR4_VMXE);

    ret = __vmxon(vcpu->vmxon_region.pa);
    if (ret == -1)
    {
        printk(KERN_INFO "VMXON failed\n");
        return -1;
    }

    vcpu->our_vmxon = ret == 0;
    return 0;
}

void hypervisor2_cpu_vmxoff(vcpu_t *vcpu)
{
    const int cpu = raw_smp_processor_id();

    if (vcpu->our_vmxon && (__vmxoff() != 0))
        printk(KERN_INFO "VMXOFF failed\n");

    /* Clear CR4.VMXE if necessary */
    if (!vcpu->old_vmxe)
        __set_cr4(__get_cr4() & ~CR4_VMXE);
}

int hypervisor2_cpu_vmclear(vcpu_t *vcpu)
{
    //const int cpu = raw_smp_processor_id();
    //printk(KERN_INFO "[%s] %s cpu %d\n", DEVICE_NAME, __func__, cpu);

    if (__vmclear(vcpu->vmcs.pa) != 0)
    {
        printk(KERN_INFO "VMCLEAR failed\n");
        return -1;
    }

    return 0;
}
