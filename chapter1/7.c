#include <stdio.h>
#include <time.h>
#include <string.h>
#include <windows.h>

void show_run_time(void)
{
    clock_t start, end, durationTime;
    int s, ms;
    start = clock();
    Sleep(1 * CLOCKS_PER_SEC); // 似乎sleep不起作用
    end = clock();
    printf("%.5d \n", start);
    printf("%.5d \n", end);
    durationTime = ((double)(end - start));
    printf("%.5d m秒\n", durationTime); //输出结果。
}

int main(int argc, char const *argv[])
{
    time_t current;
    time(&current);
    printf("ctime %s\n", ctime(&current));
    printf("asctime %s\n", asctime(localtime(&current)));

    char buff[128];
    memset(buff, '\0', 128); // 记得是单引号，表示char \0 表示结束符

    strftime(buff, 128, "时区：%z %Y 年 %m 月 %d 日", localtime(&current));
    printf("%s \n", buff);
    show_run_time();
    return 0;
}
