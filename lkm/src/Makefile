obj-m := hypervisor2.o

hypervisor2-y := main.o \
                 cpu.o \
                 device.o \
                 ioctl.o \
                 vmx.o \
                 vmm.o \
                 vmpage.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD)/lkm/src modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD)/lkm/src clean
