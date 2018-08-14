## 变长的参数

在 stdarg.h 下面提供一个 va_list 类型，它可以提取参数的变长序列，通产会手动指定序列的长度，即 count。
在通过 var_arg 读取值前，需要 start 与 end。var_arg 第一个是要读取的序列，第二个是断定的类型。

```c
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
```

## bool 支持

在 stdbool.h 里面，其实就是宏定义，0 和 1 而已。

```c
#include <stdio.h>
#include <stdbool.h>

int main(int argc, char const *argv[])
{
    bool lili_is_mail = false;
    bool lucy_is_femail = true;
    return 0;
}
```

## open 文件读写

open 打开的是文件描述符，继而再用 fdopen 打开才是流，而 fopen 打开的直接就是流。一般操作文件流的函数都是以 f 开头的，可以传递流进行操作，同样可以操作 3 个标准流，out/in/err。

```c
#include <sys/stat.h> // 文件权限 S_IREAD 等
#include <stdio.h>
#include <fcntl.h>
#include <io.h>
#include <stdlib.h>
#include <process.h>
// #include <unistd.h> linux 下的 windows 下用 io process 替代

int main(int argc, char const *argv[])
{
    int fd;
    FILE *stream;

    remove("test.txt"); // 删除文件

    fd = open("test.txt", O_CREAT | O_RDWR, S_IREAD | S_IWRITE); // 没有则创建，所有者具有读写权限

    stream = fdopen(fd, "w"); // 描述符打开

    if (stream == NULL)
    {
        abort(); // 发送 abort 信号退出
    }

    fprintf(stream, "Hello world \n");
    fclose(stream);
    system("cat test.txt"); // 执行命令，貌似测试时 execl 在 windows 没法编译。
    return 0;
}
```

## fopen 字符文件读写

将文件里面的数据逐字节读取到 stdout 上面。

```c
#include <stdio.h>

int main(int argc, char const *argv[])
{
    FILE *fp = NULL;
    char ch;
    fp = fopen("test.txt", "r");
    if (fp == NULL)
    {
        return -1;
    }

    while ((ch = fgetc(fp)) != EOF)
    {
        fputc(ch, stdout);
    }
    fclose(fp);
    return 0;
}
```

## 按字节读取

一次读取 10 个字符

```c
#include <stdio.h>

int main(int argc, char const *argv[])
{
    FILE *file = fopen("test.txt", "r");

    if (file == NULL)
    {
        printf("open error");
        return -1;
    }

    char str[10];
    fgets(str, sizeof(str), file);
    fputs(str, stdout);
    fclose(file);
    return 0;
}
```

## 结构体读取

存放的文件是二进制的格式

```c
#include <stdio.h>
#include <stdlib.h>

typedef struct _stuent
{
    char name[10];
    int number;
} student;

void reader(void)
{
    FILE *f2 = fopen("studen", "r");
    student *s2 = malloc(sizeof(student));
    fread(s2, sizeof(student), 1, f2); // 1 表示只读取一个结构体
    fclose(f2);
    printf("%s %d\t", s2->name, s2->number);
}

int main(int argc, char const *argv[])
{
    FILE *f1 = fopen("studen", "w");
    if (f1 == NULL)
    {
        perror("open error");
        return -1;
    }
    student s1 = {"Yugo", 13};
    fwrite(&s1, sizeof(student), 1, f1);
    fclose(f1);
    reader();
    return 0;
}
```

# 计算文件大小

通过计算开始到末尾的字节数获得。

```c
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
```

## 复制文件

fread

- buf 读入的数组
- 读取大小
- 读取次数
- 流

这里我们不应该一次读 128，因为我们的文字可能没有 128 那么长，空余的会乱码，所以一次读一个。num 是有读取到的个数，所以写的时候，读取到了多少，就写多少。

```c
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char const *argv[])
{
    FILE *fp_src, *fp_des;
    char buf[128];
    int num;
    fp_src = fopen("test.txt", "r");
    fp_des = fopen("test-copy.txt", "w");
    do
    {
        num = fread(buf, 1, 128, fp_src);
        fwrite(buf, 1, num, fp_des);
        if (feof(fp_src))
            break;
    } while (1);
    fclose(fp_src);
    fclose(fp_des);
    return 0;
}
```
