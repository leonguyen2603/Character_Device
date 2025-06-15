#include <stdio.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>

#define CLEAR_BUFFER _IO('L', 1)
#define GET_SIZE     _IOR('L', 2, int)

int main() {
    int fd = open("/dev/mychardev_3", O_RDWR);
    if (fd < 0) {
        perror("Failed to open device");
        return 1;
    }

    int size;

    ioctl(fd, GET_SIZE, &size);
    printf("Buffer size before clear: %d\n", size);

    ioctl(fd, CLEAR_BUFFER);
    printf("Buffer cleared.\n");

    ioctl(fd, GET_SIZE, &size);
    printf("Buffer size after clear: %d\n", size);

    close(fd);
    return 0;
}
