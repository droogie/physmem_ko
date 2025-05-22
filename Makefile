obj-m += physmem.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(shell pwd) modules
	gcc -Wall $(shell pwd)/mmap_test.c -o $(shell pwd)/mmap_test
	gcc -Wall $(shell pwd)/ioctl_test.c -o $(shell pwd)/ioctl_test
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(shell pwd) clean
	rm $(shell pwd)/mmap_test
	rm $(shell pwd)/ioctl_test

install:
	sudo mkdir -p /lib/modules/`uname -r`/kernel/drivers/physmem
	sudo cp physmem.ko /lib/modules/`uname -r`/kernel/drivers/physmem/
	sudo depmod -a
	sudo modprobe physmem
	grep -q physmem /etc/modules || echo physmem | sudo tee -a /etc/modules
