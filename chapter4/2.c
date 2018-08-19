// who | sort

#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char const *argv[])
{
    int fds[2];
    if (pipe(fds) == -1)
    {
        perror("pipe");
    }
    if (fork() == 0)
    {
        // 子进程
        char buf[128];
        dup2(fds[0], 0); // 可读连接输入 stdin
        close(fds[1]);   // 关闭写入
        execlp("sort", "sort", (char *)0);
    }
    else
    {
        if (fork() == 0)
        {
            dup2(fds[1], 1); // 可写接入 stdout
            close(fds[0]);
            execlp("who", "who", (char *)0);
        }
        else
        {
            close(fds[0]);
            close(fds[1]);
            wait(NULL);
            wait(NULL);
        }
    }
    return 0;
}
