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
        fgets(buffer, BUFFER_SIZE, stdin);
        if (feof(stdin))
            break;
        fprintf(stdout, "%s", buffer);
    }

    return 0;
}
