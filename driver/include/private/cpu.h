#pragma once

#include "vmm.h"

void vmm_cpu_id_get(struct cpuinfo_x86 *cpu_info);

bool vmm_cpu_vmx_support(void);
bool vmm_cpu_vmx_bios_support(void);

int vmm_cpu_vmxon(vcpu_t *vcpu);
int vmm_cpu_vmxoff(vcpu_t *vcpu);
int vmm_cpu_vmclear(vcpu_t *vcpu);
