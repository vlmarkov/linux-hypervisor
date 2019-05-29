#pragma once

#include "vmm.h"

void cpu_id_get(struct cpuinfo_x86 *cpu_info);

bool cpu_vmx_support(void);
bool cpu_vmx_bios_support(void);

int cpu_vmx_on(vcpu_t *vcpu);
int cpu_vmx_off(vcpu_t *vcpu);
int cpu_vm_clear(vcpu_t *vcpu);
