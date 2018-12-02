obj-m := mvmdio_uio.o

KERNELDIR ?= /home/marvell/Work2/Projects/Repo/kernel
PWD := $(shell pwd)

all:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules

clean:
	rm -rf *.o *.ko *.mod *.symvers *.order 
