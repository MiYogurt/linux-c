#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>

#define N_BITS 3

int main(int argc, char const *argv[])
{
    unsigned int i, mask = 0700;
    struct stat file_stat;
    static char *perm[] = {"---", "--x", "-w-", "-wx", "r--", "r-x", "rw-", "rwx"};
    if (stat(argv[1], &file_stat) != -1)
    {
        printf("%o\n", file_stat.st_mode);
        printf("%o\n", (file_stat.st_mode & mask));
        for (i = 3; i; --i)
        {
            printf("%3s\n", perm[(file_stat.st_mode & mask) >> (i - 1) * N_BITS]);
            mask >>= N_BITS;
            printf("mask: %o\n", mask);
        }
    }
    else
    {
        perror("stat");
    }
    return 0;
}
