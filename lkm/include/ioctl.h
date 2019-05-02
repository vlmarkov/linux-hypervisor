#pragma once

#include <linux/ioctl.h>

#define DEVICE_NAME      "hypervisor2"
#define DEVICE_PATH      "/dev/"DEVICE_NAME

#define DEVICE_MAGIC     'h'
#define DEVICE_MINOR     0
#define DEVICE_MINOR_CNT 1

typedef struct
{
    int  vmx;
    int  ncores;
    char vendor_id[16];
} hypervisor2_cpuid_t;

#define HYPERVISOR2_IOCTL_GET_CPU_ID _IOR(DEVICE_MAGIC, 0, hypervisor2_cpuid_t)
