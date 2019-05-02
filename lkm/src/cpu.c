#include "../include/private/cpu.h"

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
