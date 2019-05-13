#pragma once

#include "vmm.h"

void vmm_vmxon_setup_revision_id(vcpu_t *vcpu);
void vmm_vmcs_setup_revision_id(vcpu_t *vcpu);
void vmm_vmx_cache_capabilities_set(vcpu_t *vcpu);

int vmm_vmx_init(void);
int vmm_vmx_free(void);
