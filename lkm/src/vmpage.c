#include <linux/highmem.h>

#include "../include/private/vmm.h"


static int vmm_phys_page_init(vmpage_t *vmpage)
{
    if (!vmpage)
        return -1;

    vmpage->page = alloc_page(GFP_KERNEL | __GFP_ZERO);
    if (!vmpage->page)
        return -1;

    vmpage->p  = kmap(vmpage->page);
    vmpage->pa = page_to_phys(vmpage->page);

    return 0;
}

static int vmm_phys_page_free(vmpage_t *vmpage)
{
    if (!vmpage)
        return -1;

    kunmap(vmpage->page);
    __free_page(vmpage->page);

    vmpage->page = NULL;
    vmpage->p    = NULL;
    vmpage->pa   = 0;

    return 0;
}

int vmm_vmpage_init(vcpu_t *vcpu)
{
    int cnt = 0;

    if (vmm_phys_page_init(&vcpu->vmxon_region) == 0)
    {
        cnt++;
    }
    else
    {
        return -cnt;
    }

    if (vmm_phys_page_init(&vcpu->vmcs) == 0)
    {
        cnt++;
    }
    else
    {
        return -cnt;
    }

    if (vmm_phys_page_init(&vcpu->io_bitmap_a) == 0)
    {
        cnt++;
    }
    else
    {
        return -cnt;
    }

    if (vmm_phys_page_init(&vcpu->io_bitmap_b) == 0)
    {
        cnt++;
    }
    else
    {
        return -cnt;
    }

    if (vmm_phys_page_init(&vcpu->msr_bitmap) == 0)
    {
        cnt++;
    }
    else
    {
        return -cnt;
    }

    return cnt;
}

int vmm_vmpage_free(vcpu_t *vcpu, int cnt)
{
    if (cnt == 5)
    {
        vmm_phys_page_free(&vcpu->msr_bitmap);
        cnt--;
    }

    if (cnt == 4)
    {
        vmm_phys_page_free(&vcpu->io_bitmap_b);
        cnt--;
    }

    if (cnt == 3)
    {
        vmm_phys_page_free(&vcpu->io_bitmap_a);
        cnt--;
    }

    if (cnt == 2)
    {
        vmm_phys_page_free(&vcpu->vmxon_region);
        cnt--;
    }

    if (cnt == 1)
    {
        vmm_phys_page_free(&vcpu->vmcs);
        cnt--;
    }

    return cnt;
}
