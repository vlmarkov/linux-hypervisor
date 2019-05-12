#pragma once

#include "vmm.h"

int vmm_vmpage_init(vcpu_t *vcpu);
int vmm_vmpage_free(vcpu_t *vcpu, int cnt);
