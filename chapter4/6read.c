#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>

#define FIFO_NAME "/tmp/test_fifo"

int main(int argc, char const *argv[])
{
    int pipe_fd;
    int res;
    char buffer[4096];
    int bytes_read = 0;
    memset(buffer, '\0', sizeof(buffer));
    printf("process %d\n", getpid());
    pipe_fd = open(FIFO_NAME, O_RDONLY);
    if (pipe_fd != -1)
    {
        bytes_read = read(pipe_fd, buffer, sizeof(buffer));
        printf("read data is %s\n", buffer);
        close(pipe_fd);
    }
    else
        exit(-1);

    printf("finish\n");
    return 0;
}
