#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

#define DEVICE_NAME "/dev/embedi_char"

int main(int argc, char *argv[])
{
    char wbuff[100] = "";
    char rbuff[100] = "";
    ssize_t size = 0;
    loff_t pos = 0;
    int fd = 0;

    fd = open(DEVICE_NAME, O_RDWR);
    if (fd < 0) {
        printf("open fail\n");
        return -1;
    }

    sprintf(wbuff, "hello embedi!");
    size = write(fd, wbuff, strlen(wbuff));
    if (size < 0) {
        printf("write fail\n");
        return -1;
    }
    printf("write %ld bytes %s\n", size, wbuff);

    pos = lseek(fd, 0, SEEK_SET);
    if (pos < 0){
        printf("seek fail\n");
        return -1;
    }

    size = read(fd, rbuff, size);
    if (size < 0) {
        printf("read fail\n");
        return -1;
    }
    printf("read %ld bytes %s\n", size, rbuff);
    return 0;
}
