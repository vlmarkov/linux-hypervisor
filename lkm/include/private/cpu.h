#pragma once

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/cdev.h>

#include <linux/fs.h>

void hypervisor2_cpu_id_get(struct cpuinfo_x86 *cpu_info);