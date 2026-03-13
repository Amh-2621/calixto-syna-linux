srctree ?= $(KERNEL_SRC)

all:
	$(MAKE) -C $(srctree) M=$(PWD) modules

modules_install:
	$(MAKE) -C $(srctree) M=$(PWD) modules_install

clean:
	rm -f *.o *~ core .depend .*.cmd *.ko *.mod.c
	rm -f Module.markers Module.symvers modules.order
	rm -rf .tmp_versions Modules.symvers
