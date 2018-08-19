#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <fcntl.h>

void handler(int sig)
{
    printf("signal = %d\n", sig);
}

int main(int argc, char const *argv[])
{
    int j;
    signal(SIGPIPE, handler);
    unlink("fifo");
    mkfifo("fifo", 0644);
    pid_t pid;
    pid = fork();
    if (pid == 0)
    {
        int fd;
        fd = open("fifo", O_RDONLY);
        close(fd); // 子进程关闭读端
    }
    else
    {
        int fd;
        fd = open("fifo", O_WRONLY);
        int ret;
        sleep(1);                    // 等待读端关闭
        ret = write(fd, "hello", 5); // 写入失败，接受到 sigpipe 信号
        printf("ret=%d\n", ret);
    }
    return 0;
}
