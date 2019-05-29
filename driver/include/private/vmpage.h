#pragma once

#include "vmm.h"

int vmpage_init(vcpu_t *vcpu);
int vmpage_free(vcpu_t *vcpu, int cnt);
