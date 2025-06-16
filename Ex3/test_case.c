#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <string.h>

#define DEVICE "/dev/mychardev_3"

#define CLEAR_BUFFER _IO('L', 1)
#define GET_SIZE     _IOR('L', 2, int)

int main() {
    // Open the device file
    int fd = open(DEVICE, O_RDWR);
    if (fd < 0) 
    {
        perror("open");
        return 1;
    }

    // Write test
    char *data = "Hello from user!";
    write(fd, data, strlen(data));
    printf("Written to device: %s\n", data);

    // Read test
    char buf[1024] = {0};
    lseek(fd, 0, SEEK_SET);  // Reset offset
    read(fd, buf, sizeof(buf));
    printf("Read from device: %s\n", buf);

    // IOCTL: GET_SIZE
    int size = 0;
    if (ioctl(fd, GET_SIZE, &size) == -1) 
    {
        perror("ioctl GET_SIZE");
    } 
    else 
    {
        printf("Buffer size: %d\n", size);
    }

    // IOCTL: CLEAR_BUFFER
    if (ioctl(fd, CLEAR_BUFFER) == -1) 
    {
        perror("ioctl CLEAR_BUFFER");
    } 
    else 
    {
        printf("Buffer cleared!\n");
    }

    // IOCTL: GET_SIZE again
    if (ioctl(fd, GET_SIZE, &size) == -1) 
    {
        perror("ioctl GET_SIZE");
    } 
    else 
    {
        printf("Buffer size after clear: %d\n", size);
    }

    close(fd);
    return 0;
}
