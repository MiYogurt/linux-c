#include <stdio.h>
#include <stdlib.h>

int main(int argc, char const *argv[])
{
    FILE *file = fopen("test.txt", "r");
    rewind(file);                      // 调准指针到开头，可去掉
    fseek(file, 0, SEEK_END);          // 修改到末尾
    printf("%d byte \n", ftell(file)); // 开始到当前指针的长度。
    return 0;
}
