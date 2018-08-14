#include <sys/stat.h>
#include <stdio.h>
#include <fcntl.h>
#include <io.h>
#include <stdlib.h>
#include <process.h>
// #include <unistd.h>

int main(int argc, char const *argv[])
{
    int fd;
    FILE *stream;

    remove("test.txt");

    fd = open("test.txt", O_CREAT | O_RDWR, S_IREAD | S_IWRITE);

    stream = fdopen(fd, "w");

    if (stream == NULL)
    {
        abort();
    }

    fprintf(stream, "Hello world \n");
    fclose(stream);
    system("cat test.txt");
    return 0;
}