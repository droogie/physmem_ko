#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <stdint.h>

struct io_req {
        uint8_t  size;
        uint16_t port;
        uint32_t data;
};

#define IOCTL_PA_ALLOC _IOWR('p', 0, size_t *)
#define IOCTL_PA_FREE  _IOR('p', 1, void *)
#define IOCTL_IO_READ  _IOR('p', 2, struct io_req)
#define IOCTL_IO_WRITE _IOR('p', 3, struct io_req)

#define IO_SIZE_BYTE  0
#define IO_SIZE_WORD  1
#define IO_SIZE_DWORD 2

int fd;

void allocate_physical_memory() {
    int ret;
    unsigned long size = 4096;
    printf("Informing driver to allocate\n");
    ret = ioctl(fd, IOCTL_PA_ALLOC, (size_t*) &size); 
    printf("ioctl ret: %x\n", ret);
    printf("addr: %p\n", (void *) size);

    printf("Informing driver to free allocation\n");
    ret = ioctl(fd, IOCTL_PA_FREE, (void *) &size);

    printf("ioctl ret: %x\n", ret);
}

void io_access() {
    int ret;
    printf("Accessing IO Port\n");
    struct io_req io = {
        .size = IO_SIZE_BYTE,
        .port = 0x64,
        .data = 0xaa,
    };

    printf("Initializing self test...\n");
    printf("IO Port W 0x%02x: 0x%x\n", io.port, io.data);
    ret = ioctl(fd, IOCTL_IO_WRITE, &io); 
    printf("ioctl ret: %x\n", ret);

    usleep(5000);

    io.port = 0x60;
    printf("Keyboard response code\n");
    ret = ioctl(fd, IOCTL_IO_READ, &io); 
    printf("ioctl ret: %x\n", ret);
    printf("IO Port R 0x%02x: 0x%x\n", io.port, io.data);
    if (io.data == 0x55) {
        printf("Keyboard self test succeeded\n");
    } else {
        printf("Keyboard self test failed\n");
    }
}

int main() {
    printf("\nOpening Driver\n");
    fd = open("/dev/physmem", O_RDWR);
    if(fd < 0) {
        printf("Cannot open device file...\n");
        return 0;
    }

    //allocate_physical_memory();
    io_access();

    printf("Closing Driver\n");
    close(fd);
}

