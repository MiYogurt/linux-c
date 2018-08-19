# 重定向

将标准输出 stdout 定向到文件 fd 里面去。 通过 dup2 实现。

```c
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

#define BUFFER_SIZE 1024

// ./a.out test.tmp
// ctrl + d 输入 eof

int main(int argc, char const *argv[])
{
    int fd;
    char buffer[BUFFER_SIZE];
    fd = open(argv[1], O_TRUNC | O_RDWR | O_CREAT, 0755);

    if (fd < 0)
    {
        perror("open");
    }

    if (dup2(fd, fileno(stdout)) == -1)
    {
        perror("dup2");
    }

    while (1)
    {
        fgets(buffer, BUFFER_SIZE, stdin); // 等待用户输入
        if (feof(stdin))
            break;
        fprintf(stdout, "%s", buffer);
    }

    return 0;
}
```

## 管道

一般对于进程来说，子进程死完了就成了僵死进程，而父进程死了，子进程就成了孤儿进程。

实现 `who | sort`，通过 pipe 创建临时管道，在进程间通信。

```c
// who | sort

#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char const *argv[])
{
    int fds[2];
    if (pipe(fds) == -1) // 创建管道
    {
        perror("pipe");
    }
    if (fork() == 0)
    {
        // 子进程
        char buf[128];
        dup2(fds[0], 0); // 可读连接输入 stdin
        close(fds[1]);   // 关闭写入
        execlp("sort", "sort", (char *)0); // sort 从 fds[0] 读入数据
    }
    else
    {
        if (fork() == 0)
        {
            dup2(fds[1], 1); // 可写接入 stdout
            close(fds[0]);
            execlp("who", "who", (char *)0); // 执行命令 who 结果输入 fds[1]
        }
        else
        {
            close(fds[0]);
            close(fds[1]);
            wait(NULL); // 等待子进程
            wait(NULL);
        }
    }
    return 0;
}
```

通过 popen 来对管道进行操作。 `echo test | cat`。

```c
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>

int main(int argc, char const *argv[])
{
    FILE *finput, *foutput;

    char buffer[PIPE_BUF];
    int n;
    finput = popen("echo test!", "r");
    foutput = popen("cat", "w");
    read(fileno(finput), buffer, strlen("test"));
    write(fileno(foutput), buffer, strlen("test"));
    pclose(finput);
    pclose(foutput);
    printf("\n");
    return 0;
}
```

现在使用 mknod 创建的有名管道 fifo 文件，也可以通过 mkfifo 创建。它是一种记录内存中数据的文件，但是两个进程结束后，内存里面数据会丢失。

必须等两端都就绪了才开始通信，否则某一端就会阻塞。当读端先断开，写端会接受到 signpipe 信号。写端先断开，读端最后会读到 0 。

```c
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
```

## 进程间通信

写端，写入 buffer 到 pipe_fd 里面去。

```c
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
```

读端

```c
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
```
