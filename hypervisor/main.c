#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h> 
#include <sys/ioctl.h>

#include "../lkm/hypervisor2.h"

void ioctl_get_msg(int fd)
{
    struct hypervisor2_cpuid cpuid;
    memset(&cpuid, 0, sizeof(struct hypervisor2_cpuid));

    int ret_val = ioctl(fd, HYPERVISOR2_IOCTL_GET_CPU_ID, &cpuid);
    if (ret_val < 0)
    {
        printf ("ioctl_get_msg failed:%d\n", ret_val);
        exit(-1);
    }

    printf("VMX support: %s\n", cpuid.vmx ? "enable" : "disable");
    printf("Vendor id  : %s\n", cpuid.vendor_id);
    return;
}

int main(int argc, char const *argv[])
{
    int fd = open("/dev/hypervisor2", O_RDONLY);
    if (fd < 0)
    {
        printf ("Can't open device file: %s\n", DEVICE_NAME);
        return -1;
    }

    ioctl_get_msg(fd);

    close(fd);
    return 0;
}
