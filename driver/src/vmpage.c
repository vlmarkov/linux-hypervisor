#include <linux/highmem.h>

#include "../include/private/vmm.h"

static int vmpage_phys_page_init(vmpage_t *vmpage)
{
    if (IS_ERR(vmpage))
        return -EFAULT;

    vmpage->page = alloc_page(GFP_KERNEL | __GFP_ZERO);
    if (IS_ERR(vmpage->page))
        return -ENOMEM;

    vmpage->p  = kmap(vmpage->page);
    vmpage->pa = page_to_phys(vmpage->page);

    return 0;
}

static int vmpage_phys_page_free(vmpage_t *vmpage)
{
    if (IS_ERR(vmpage))
        return -EFAULT;

    kunmap(vmpage->page);
    __free_page(vmpage->page);

    vmpage->page = NULL;
    vmpage->p    = NULL;
    vmpage->pa   = 0;

    return 0;
}

int vmpage_init(vcpu_t *vcpu)
{
    int cnt = 0;

    if (IS_ERR(vcpu))
        return -EFAULT;

    if (vmpage_phys_page_init(&vcpu->vmxon_region) == 0)
    {
        cnt++;
    }
    else
    {
        return -cnt;
    }

    if (vmpage_phys_page_init(&vcpu->vmcs) == 0)
    {
        cnt++;
    }
    else
    {
        return -cnt;
    }

    if (vmpage_phys_page_init(&vcpu->io_bitmap_a) == 0)
    {
        cnt++;
    }
    else
    {
        return -cnt;
    }

    if (vmpage_phys_page_init(&vcpu->io_bitmap_b) == 0)
    {
        cnt++;
    }
    else
    {
        return -cnt;
    }

    if (vmpage_phys_page_init(&vcpu->msr_bitmap) == 0)
    {
        cnt++;
    }
    else
    {
        return -cnt;
    }

    return cnt;
}

int vmpage_free(vcpu_t *vcpu, int cnt)
{
    if (IS_ERR(vcpu))
        return -EFAULT;

    if (cnt == 5)
    {
        vmpage_phys_page_free(&vcpu->msr_bitmap);
        cnt--;
    }

    if (cnt == 4)
    {
        vmpage_phys_page_free(&vcpu->io_bitmap_b);
        cnt--;
    }

    if (cnt == 3)
    {
        vmpage_phys_page_free(&vcpu->io_bitmap_a);
        cnt--;
    }

    if (cnt == 2)
    {
        vmpage_phys_page_free(&vcpu->vmxon_region);
        cnt--;
    }

    if (cnt == 1)
    {
        vmpage_phys_page_free(&vcpu->vmcs);
        cnt--;
    }

    return cnt;
}
