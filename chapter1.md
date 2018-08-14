在 C 语言中，数组的值保存的其实是开始位置的地址，这个示例演示了通过 `malloc` 请求生成一个长度为 10 的数组内存空间。它接受申请的字节数 `10 * sizeof(int)`，假如返回值等于 NULL 说明无法申请，这里通过强制转换`（int *）` 再赋值给了 `array`。

现在我们拿 array 指针像数组一样去使用。当然也可以 `*(array+i)` 去访问具体的值。

使用完，释放内存则使用 `free` ，这样可以避免内存泄漏。

```c
#include <stdio.h>
#include <stdlib.h> // 提供 free malloc

int main(int argc, char const *argv[])
{
    int *array;

    if ((array = (int *)malloc(10 * sizeof(int))) == NULL)
    {
        printf("malloc memory unsuccessful");
        exit(1);
    }

    for (int i = 9; i >= 0; i--)
    {
        array[i] = i;
    }

    for (int i = 9; i >= 0; i--)
    {
        printf("%d \n", array[i]);
    }

    free(array);
    array = NULL;
    return 0;
}
```

`realloc` 可以再分配，老的指针会被复用无需手动释放。

```c
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char const *argv[])
{
    int *arr;
    arr = (int *)malloc(2 * sizeof(int));
    if (arr == NULL)
    {
        printf("malloc error!");
        exit(-1);
    }
    arr[0] = 0;
    arr[1] = 1;

    arr = (int *)realloc(arr, 5 * sizeof(int));
    arr[2] = 2;

    for (int i = 0; i < 5; i++)
    {
        printf("%d \t", arr[i]);
    }
    return 0;
}
```

结果 `0 1 2 0 0`

`calloc` 比 `malloc` 稍微高级点，以下两种方式等价。

```c
int *arr1 = (int *)calloc(4, sizeof(int));
int *arr2 = (int *)malloc(4 * sizeof(int));
```

这些都是在堆上申请内存空间。函数调用栈，函数的调用执行是有计算机控制的，当一个函数执行完了会回退，所以栈是由计算机控制的，而堆是由编程者控制的。

## 命令行参数解析

这里我测试的是 windows 环境，由于没有 getopt.h 所以我就想找一个替代品，于是我找到了 `https://github.com/skeeto/optparse` ，实例程序如下，这个指定参数的风格不需要等号，稍微有些不习惯。编译运行以下命令。

```bash
./chapter1/4.exe -a -b -d60 -c blue hello world
```

```c
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h> // 添加 bool 支持
#define OPTPARSE_IMPLEMENTATION
#define OPTPARSE_API static
#include "optparse.h" // 将改文件从 github 下载下来丢在同级目录即可

int main(int argc, char **argv)
{
    bool amend = false;
    bool brief = false;
    const char *color = "white";
    int delay = 0;

    char *arg;
    int option;
    struct optparse options; // 新建结构体全局变量，存储当前解析值

    optparse_init(&options, argv); // 初始化
    while ((option = optparse(&options, "abc:d::")) != -1) // 解析选项 abcd, 冒号表示有输入，会赋值给optarg，双冒号代表可选。
    {
        switch (option)
        {
        case 'a':
            amend = true;
            break;
        case 'b':
            brief = true;
            break;
        case 'c':
            color = options.optarg;
            break;
        case 'd':
            delay = options.optarg ? atoi(options.optarg) : 1;
            break;
        case '?': // 解析错误
            fprintf(stderr, "%s: %s\n", argv[0], options.errmsg);
            exit(EXIT_FAILURE);
        }
    }

    printf("amend = %s \n", amend ? "true" : "false");
    printf("brief = %s \n", brief ? "true" : "false");
    printf("color = %s \n", color);
    printf("delay = %d \n", delay);

    while ((arg = optparse_arg(&options))) // 解析命令
        printf("%s\n", arg);
    return 0;
}
```

## 对环境变量进行更改

还有 `unset("NAME")` 进行删除，只在单次运行有效，全局还是要修改 bashrc 等文件。

```c
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
```

## 时间

`clock` 可以获得时间，通常用来计算差值得到运行时间， `time` 也可以获得时间，只不过是秒数，经过 `ctime`处理秒数，或者 `asctime` 处理 `tm`结构体 ，可以变成人易读的格式 `Day Mon dd hh:mm:ss yyyy`。

假如想获得年月分秒具体数值，需要得到 `tm` 结构体，`gmtime` 和 `localtime` 可以将秒数处理成 `tm` 结构体。

自定义还可以用 `strftime` 进行定制，`memset` 表示设置该内存存放值，可理解为初始化，申请内存。格式化的符号，比如 `%m` 可以看 http://devdocs.io/c/chrono/strftime 的 format 选项。

```c
#include <stdio.h>
#include <time.h>
#include <string.h>

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
    return 0;
}
```
