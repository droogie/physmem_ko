obj-m += physmem.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
	gcc $(PWD)/mmap_test.c -o $(PWD)/mmap_test
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
	rm $(PWD)/mmap_test
