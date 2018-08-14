#include <stdio.h>
#include <stdlib.h>

int main(int argc, char const *argv[])
{
    printf("%s \n", getenv("HOSTNAME"));
    putenv("NAME=Yugo");
    setenv("NAME", "MiYogurt", 1); // 第三个参数非 0 表覆盖
    printf("%s \n", getenv("NAME"));
    return 0;
}
