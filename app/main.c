#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h> 
#include <sys/ioctl.h>

#include "../driver/include/ioctl.h"

void ioctl_get_msg(int fd)
{
    hypervisor_id_t hypervisor_id;
    memset(&hypervisor_id, 0, sizeof(hypervisor_id_t));

    int ret_val = ioctl(fd, HYPERVISOR_IOCTL_GET_ID, &hypervisor_id);
    if (ret_val < 0)
    {
        printf ("ioctl_get_msg failed:%d\n", ret_val);
        exit(-1);
    }

    printf("Cores       : %d\n", hypervisor_id.ncores);
    printf("VMX support : %s\n", hypervisor_id.vmx ? "enable" : "disable");
    printf("Vendor id   : %s\n", hypervisor_id.vendor_id);
    return;
}

int main(int argc, char const *argv[])
{
    int fd = open(DEVICE_PATH, O_RDONLY);
    if (fd < 0)
    {
        printf("Can't open device file: %s\n", DEVICE_PATH);
        return -1;
    }

    ioctl_get_msg(fd);

    close(fd);
    return 0;
}
