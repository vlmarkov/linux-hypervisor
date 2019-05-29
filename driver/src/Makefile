obj-m := hypervisor.o

hypervisor-y := main.o \
                cpu.o \
                device.o \
                ioctl.o \
                vmx.o \
                vmm.o \
                vmpage.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD)/driver/src modules
	cp $(PWD)/driver/src/*.ko $(PWD)/build/
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD)/driver/src clean
