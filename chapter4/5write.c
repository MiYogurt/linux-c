#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <limits.h>
#include <sys/stat.h>
#include <sys/types.h>

#define FIFO_NAME "/tmp/test_fifo"

int main(int argc, char const *argv[])
{
    int pipe_fd;
    int res;
    char buffer[] = "hello world!";
    if (access(FIFO_NAME, F_OK) == -1)
    {
        res = mkfifo(FIFO_NAME, 0766);
        if (res != 0)
        {
            perror("mkfifo");
        }
    }
    printf("process startd %d\n", getpid());
    pipe_fd = open(FIFO_NAME, O_WRONLY);
    if (pipe_fd != -1)
    {
        res = write(pipe_fd, buffer, sizeof(buffer));
        if (res == -1)
        {
            perror("write res error");
        }
        close(pipe_fd);
    }
    else
        exit(-1);
    return 0;
}
