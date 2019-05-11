#pragma once

#include "vmm.h"

void hypervisor2_vmxon_setup_revision_id(vcpu_t *vcpu);
void hypervisor2_vmcs_setup_revision_id(vcpu_t *vcpu);
void hypervisor2_vmx_cache_capabilities_set(vcpu_t *vcpu);

int hypervisor2_vmx_init(void);
int hypervisor2_vmx_free(void);
