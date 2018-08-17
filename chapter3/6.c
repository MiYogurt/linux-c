#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <utime.h>
#include <time.h>
#include <sys/types.h>

int main(int argc, char const *argv[])
{
    struct utimbuf buf;
    char *ptr;
    time_t tm;
    time(&tm);
    buf.actime = tm - 1;
    buf.modtime = tm - 2;
    ptr = ctime(&tm);
    printf("now %s\n", ptr);
    ptr = ctime(&buf.actime);
    printf("actime %s\n", ptr);
    ptr = ctime(&buf.modtime);
    printf("modtime %s\n", ptr);
    utime(argv[1], &buf);
    return 0;
}
