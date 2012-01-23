KERNDIR := /lib/modules/`uname -r`/build
KERNEL_SOURCES := $(KERNDIR)
obj-m := hello-1.o
EXTRA_CFLAGS += -Os
PWD := $(shell pwd)
default:
	make -C $(KERNEL_SOURCES) M=$(PWD) modules
clean:
	make -C $(KERNEL_SOURCES) M=$(PWD) clean