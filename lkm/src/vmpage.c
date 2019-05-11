#include <linux/highmem.h>

#include "../include/private/vmm.h"

static int hypervisor2_vmpage_allocate_(vmpage_t *p)
{
    p->page = alloc_page(GFP_KERNEL | __GFP_ZERO);
    if (!p->page)
            return -1;

    p->p = kmap(p->page);
    p->pa = page_to_phys(p->page);
    return 0;
}

static void hypervisor2_vmpage_free_(vmpage_t *p)
{
    kunmap(p->page);
    __free_page(p->page);
    p->page = NULL;
    p->p = NULL;
    p->pa = 0;
}

void hypervisor2_vmpage_free(vcpu_t *vcpu, int cnt)
{
    if (cnt == 5) { hypervisor2_vmpage_free_(&vcpu->msr_bitmap); cnt--; }
    if (cnt == 4) { hypervisor2_vmpage_free_(&vcpu->io_bitmap_b); cnt--; }
    if (cnt == 3) { hypervisor2_vmpage_free_(&vcpu->io_bitmap_a); cnt--; }
    if (cnt == 2) { hypervisor2_vmpage_free_(&vcpu->vmxon_region); cnt--; }
    if (cnt == 1) { hypervisor2_vmpage_free_(&vcpu->vmcs); cnt--; }
}

int hypervisor2_vmpage_allocate(vcpu_t *vcpu)
{
    int cnt = 0;
    if (hypervisor2_vmpage_allocate_(&vcpu->vmxon_region) == 0) cnt++; else return -cnt;
    if (hypervisor2_vmpage_allocate_(&vcpu->vmcs) == 0) cnt++; else return -cnt;
    if (hypervisor2_vmpage_allocate_(&vcpu->io_bitmap_a) == 0) cnt++; else return -cnt;
    if (hypervisor2_vmpage_allocate_(&vcpu->io_bitmap_b) == 0) cnt++; else return -cnt;
    if (hypervisor2_vmpage_allocate_(&vcpu->msr_bitmap) == 0) cnt++; else return -cnt;

    return cnt;
}
