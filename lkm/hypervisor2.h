#pragma once

#include <linux/ioctl.h>

#define DEVICE_NAME      "hypervisor2"
#define DEVICE_MINOR     0
#define DEVICE_MINOR_CNT 1

struct hypervisor2_cpuid
{
    int  vmx;
    char vendor_id[16];
};

#define HYPERVISOR2_IOCTL_GET_CPU_ID _IOR('h', 0, struct hypervisor2_cpuid)
