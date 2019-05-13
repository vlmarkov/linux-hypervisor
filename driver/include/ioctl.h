#pragma once

#include <linux/ioctl.h>

#define MODULE_NAME      "SIMPLE-VMM"
#define DEVICE_PATH      "/dev/"MODULE_NAME

#define DEVICE_MAGIC     'h'
#define DEVICE_MINOR     0
#define DEVICE_MINOR_CNT 1

typedef struct
{
    int  vmx;
    int  ncores;
    char vendor_id[16];
} vmm_cpuid_t;

#define VMM_IOCTL_GET_CPU_ID _IOR(DEVICE_MAGIC, 0, vmm_cpuid_t)
