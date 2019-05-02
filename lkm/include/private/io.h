#pragma once

int hypervisor2_open(struct inode *inode, struct file *file);
int hypervisor2_release(struct inode *inode, struct file *file);

ssize_t hypervisor2_read(struct file *filp, char __user *buf, size_t len, loff_t *off);
ssize_t hypervisor2_write(struct file *filp, const char __user *buf, size_t len, loff_t *off);
