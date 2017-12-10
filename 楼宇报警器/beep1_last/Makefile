ifeq ($(KERNELRELEASE),)
##KERNEL_DIR ?= /lib/modules/$(shell uname -r)/build

KERNEL_DIR ?= /home/linux/linux-3.14/
PWD ?= $(shell pwd)
modules:
	make -C $(KERNEL_DIR) M=$(PWD) modules

end:
	make -C $(KERNEL_DIR) M=$(PWD) clean
else

obj-m += beep.o
endif
