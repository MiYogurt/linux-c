#include <stdio.h>
#include <stdarg.h>

/*
* cout 计算的长度
*/
int add_nums(int count, ...)
{
    int result = 0;
    va_list args;
    va_start(args, count);
    for (int i = 0; i < count; ++i)
    {
        result += va_arg(args, int);
    }
    va_end(args);
    return result;
}

int main(void)
{
    printf("%d \n", add_nums(5, 2, 3, 4, 5, 6));
    return 0;
}
