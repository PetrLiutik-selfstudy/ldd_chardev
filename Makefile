obj-m := chardev_lkm.o

KERNEL_DIR ?= /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

modules:
	make -C$(KERNEL_DIR) M=$(PWD)

clean:
	make -C$(KERNEL_DIR) M=$(PWD) clean

