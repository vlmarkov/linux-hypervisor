#pragma once

#include "vmm.h"

int vmx_set_vmxon_revision_id(vcpu_t *vcpu);
int vmx_set_vmcs_revision_id(vcpu_t *vcpu);
int vmx_set_cache_capabilities(vcpu_t *vcpu);
