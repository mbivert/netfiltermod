obj-m += sample_module_for_techblog.o 
all: 
	make -C /lib/modules/`uname -r`/build/ M=`pwd` modules 
clean: 
	make -C /lib/modules/`uname -r`/build/ M=`pwd` clean
clean-files := Module.symvers

