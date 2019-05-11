#pragma once

#include "vmm.h"

void hypervisor2_vmpage_free(vcpu_t *vcpu, int cnt);
int hypervisor2_vmpage_allocate(vcpu_t *vcpu);
