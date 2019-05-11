#pragma once

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/cdev.h>

#include <linux/fs.h>

#include "vmm.h"

void hypervisor2_cpu_id_get(struct cpuinfo_x86 *cpu_info);

bool hypervisor2_cpu_vmx_support(void);
bool hypervisor2_cpu_vmx_bios_support(void);

int hypervisor2_cpu_vmxon(vcpu_t *vcpu);
void hypervisor2_cpu_vmxoff(vcpu_t *vcpu);
int hypervisor2_cpu_vmclear(vcpu_t *vcpu);
