## /dev/physmem

An unrestricted device to map physical memory to userland. I created this to load on machines that were compiled with `STRICT_DEVMEM`, which restricts mapping physical memory beyond 1mb.

## Build

```
make
sudo insmod physmem.ko
sudo ./mmap_test
```

## Unload

```
sudo rmmod physmem
```



