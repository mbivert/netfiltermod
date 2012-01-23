KERNDIR := /lib/modules/`uname -r`/build
KERNEL_SOURCES := $(KERNDIR)
obj-m := netfiltermod-ex.o
EXTRA_CFLAGS += -Os
PWD := $(shell pwd)
default:
	make -C $(KERNEL_SOURCES) M=$(PWD) modules
