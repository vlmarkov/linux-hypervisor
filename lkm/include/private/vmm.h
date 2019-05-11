#pragma once

#include <linux/module.h>

typedef struct vmpage
{
    // Public fields
    char *p;
    uintptr_t pa;

    // Private fields
    struct page *page;
} vmpage_t;

typedef struct vcpu
{
    /* Cached VMX capabilities */
    u64 ia32_vmx_basic;
    u32 vmcs_revision_id;
    bool has_true_ctls;

    u64 ia32_vmx_pinbased_ctls;
    u64 ia32_vmx_true_pinbased_ctls;
    u32 pinbased_allowed0;
    u32 pinbased_allowed1;

    u64 ia32_vmx_procbased_ctls;
    u64 ia32_vmx_true_procbased_ctls;
    u32 procbased_allowed0;
    u32 procbased_allowed1;

    u64 ia32_vmx_exit_ctls;
    u64 ia32_vmx_true_exit_ctls;
    u32 exit_ctls_allowed0;
    u32 exit_ctls_allowed1;

    u64 ia32_vmx_entry_ctls;
    u64 ia32_vmx_true_entry_ctls;
    u32 entry_ctls_allowed0;
    u32 entry_ctls_allowed1;

    vmpage_t vmxon_region;
    vmpage_t vmcs;

    vmpage_t io_bitmap_a;
    vmpage_t io_bitmap_b;
    vmpage_t msr_bitmap;

    u64 old_vmxe;
    bool our_vmxon;
    bool may_free;
} vcpu_t;

typedef struct vmm
{
    int size;
    vcpu_t *vcpu;
} vmm_t;

int hypervisor2_vmm_init(void);
int hypervisor2_vmm_free(void);

void hypervisor2_vmm_start(void *unused);
void hypervisor2_vmm_stop(void *unused);
