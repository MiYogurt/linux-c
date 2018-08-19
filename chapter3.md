这一节开始，所有的代码都是在 linux 下面运行的。

## 读取目录下的文件名

都在 dirent 头文件里面，opendir 可以打开一个目录，通过 readdir 不停的得到一个结构体，打开了还需要 closedir 关闭。

文档 http://man7.org/linux/man-pages/man3/readdir.3.html

> 如何找文档？使用 bing 的站内搜素功能修改 opendir 为你想要的函数即可。
> https://cn.bing.com/search?q=site:man7.org%20opendir

```c
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
// #include <direct.h>

int main(int argc, char const *argv[])
{
	DIR *dirp;
	struct dirent *dp;
	dirp = opendir(argv[1]);
	while ((dp = readdir(dirp)) != NULL)
	{
		printf("name %s \n", dp->d_name);
	}
	closedir(dirp);
	return 0;
}
```

## 对目录进行一些操作

切换目录，创建目录，删除目录，拿到当前路径，getcwd 和 get_current_dir_name 一致，只是参数不同。

```c
#include <stdio.h>
#include <stdlib.h>
// #include <direct.h>
#include <dirent.h>

int main(void)
{
    chdir("/home/yugo");
    printf("cwd: %s\n", getcwd(NULL, 128));
    printf("current: %s\n", get_current_dir_name());
    mkdir("new_folder");
    mkdir("new_folder2");
    rmdir("new_folder2");
    return 0;
}
```

## 实现 cp 命令

- copy_dir 是复制路径
- copy_file 是复制文件

strcmp 可以比较两个字符串，为 0 表示他们相等，0 表示偏移量，无偏移代表全部都对上。

stat 函数可以拿到文件或者目录的描述信息，它返回一个结构体，拿到 st_mode 值，可以通过 S_ISDIR 判断是否是目录，S_ISREG 是否是普通文件。方法都在 sys/stat 头文件里面。

st_mode 其实就是 linux 文件描述符，它包含文件的类型和文件的权限信息，都是用数字表示的。

fcntl 里面包含了一些文件文件控制的东西，比如权限，标志位变量，比如 O_WRONLY 就从里面来的，O 表前缀，Open，WR 代表 Write 后面即 only，多个权限通过 | 连接。其实就是位运算。

每次发生错误可以从 errno 拿到错误号，perror 会自动帮你 strerror(errno)，得到错误号的文字描述。

在 c 语言中，错误一般会是 -1。

```c
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>

int copy_dir(const char *src, const char *dst)
{
    DIR *dirp = NULL;

    if (NULL == (dirp = opendir(src)))
    {
        perror("open error");
        exit(EXIT_FAILURE);
    }

    struct dirent *entp = NULL; // 每次遍历的文件指针

    while (NULL != (entp = readdir(dirp)))
    {
        if (strcmp(entp->d_name, "..") == 0 || strcmp(entp->d_name, ".") == 0)
        {
            continue;
        }

        char *name_src = (char *)malloc(strlen(src) + strlen(entp->d_name) + 2); // 申请一些预留空间
        sprintf(name_src, "%s/%s\0", src, entp->d_name);
        char *name_dst = (char *)malloc(strlen(dst) + strlen(entp->d_name) + 2);
        sprintf(name_dst, "%s/%s\0", dst, entp->d_name);

        struct stat stat_src;

        if (stat(name_src, &stat_src) == -1)
        {
            fprintf(stderr, "%s(%d): stat error (%s)!\n", __FILE__, __LINE__, strerror(errno));
            exit(EXIT_FAILURE);
        }

        if (S_ISREG(stat_src.st_mode))
        {
            copy_file(name_src, name_dst, stat_src.st_mode);
        }

        else if (S_ISDIR(stat_src.st_mode)) // 是目录递归调用

        {
            if (-1 == mkdir(name_dst, stat_src.st_mode))
            {
                perror("mkdir error");
                exit(EXIT_FAILURE);
            }
            copy_dir(name_src, name_dst);
        }

        free(name_src);
        free(name_dst);
    }
    return 0;
}

int copy_file(const char *src, const char *dst, long st_mode)
{
    int num;
    char buf[128];
    int fd_dst = open(dst, O_CREAT | O_WRONLY, st_mode);
    int fd_src = open(src, O_RDONLY, st_mode);

    if (fd_src == -1)
    {
        perror("open src error");
        exit(EXIT_FAILURE);
    }

    if (fd_dst == -1)
    {
        perror("open dst error");
        exit(EXIT_FAILURE);
    }

    do
    {
        num = read(fd_src, buf, 128);
        write(fd_dst, buf, num);
    } while (num == 128);
    close(fd_dst);
    close(fd_src);
    return 0;
}

int main(int argc, char const *argv[])
{
    struct stat stat_src;
    if (stat(argv[1], &stat_src) != 0)
    {
        fprintf(stderr, "%s(%d): stat error(%s)\n", __FILE__, __LINE__, strerror(errno));
        exit(EXIT_FAILURE);
    }

    umask(0000); // 权限掩码，跟子网掩码一样，跟权限做减法，0000 表示保持原有权限，比如设置 777 则 777 - 000 还是 777

    if (S_ISREG(stat_src.st_mode)) // 源文件是普通文件
    {
        struct stat stat_dst;
        if (stat(argv[2], &stat_dst) == -1) // 出错了，由于没有该文件存在引起的，直接复制，复制函数里面会自动创建。
        {
            if (errno != ENOENT) // 当不是由于不存在的文件报错，说明真的有错误
            {
                fprintf(stderr, "%s(%d): stat error(%s)\n", __FILE__, __LINE__, strerror(errno));
                exit(EXIT_FAILURE);
            }
            else
            {
                copy_file(argv[1], argv[2], stat_src.st_mode);
            }
        }
        else // 文件属性存在，则需要判断是目录还是什么
        {
            if (S_ISDIR(stat_dst.st_mode)) // 是目录
            {
                char *ptr = (char *)malloc(strlen(argv[1]) + strlen(argv[2]) + 2);
                sprintf(ptr, "%s/%s\0", argv[2], argv[1]);
                copy_file(argv[1], ptr, stat_src.st_mode);
            }
            else // 说明存在一个同名文件
            {
                printf("file %s exist, overwrite it[y/n]?", argv[2]);
                char op;
                while (1)
                {
                    op = getchar();
                    if (op == 'y' || op == 'Y' || op == 'n' || op == 'N')
                        break;
                }
                if (op == 'y' || op == 'Y') // 是否覆盖
                {
                    copy_file(argv[1], argv[2], stat_src.st_mode);
                    return 0;
                }
                return 0;
            }
        }
    }

    else if (S_ISDIR(stat_src.st_mode)) // 源文件是目录
    {
        struct stat stat_dst;
        if (stat(argv[2], &stat_dst) == -1) // 有错误说明目标不存在，可能需要新建
        {
            if (errno != ENOENT) // 报错不是由于不存在引起的
            {
                fprintf(stderr, "%s(%d): stat error(%s)\n", __FILE__, __LINE__, strerror(errno));
                exit(EXIT_FAILURE);
            }
            else
            {
                errno = 0;
                if (-1 == mkdir(argv[2], stat_src.st_mode))
                {
                    perror("mkdir error");
                    exit(EXIT_FAILURE);
                }
                copy_dir(argv[1], argv[2]); // 创建目录复制到该目录
            }
        }
        else if (S_ISREG(stat_dst.st_mode)) // 目标是文件
        {
            fprintf(stderr, "dst is file but src is folder");
            exit(EXIT_FAILURE);
        }
        else // 目标地址是目录
        {
            char *ptr = (char *)malloc(strlen(argv[1]) + strlen(argv[2]) + 2);
            sprintf(ptr, "%s/%s\0", argv[2], argv[1]);
            printf("%s", ptr);
            if (-1 == mkdir(ptr, stat_src.st_mode))
            {
                perror("mkdir");
                exit(EXIT_FAILURE);
            }
            copy_dir(argv[1], ptr);
            free(ptr);
        }
    }

    return 0;
}
```

文件类型

- - 普通文件
- d 目录
- c 字符设备 串口
- b 块设备 驱动内存
- l 符号链接
- s socket
- p 管道文件 试试 `mknod test p`

## 计算文件权限

首先我们要懂得权限，权限都是 8 进制，4 2 1 对应 rwx。及 100 为 4 ， 010 为 2， 001 为 1。

而且有拥有者、拥有组、其他人，三个，通常我们会说数字版本的即 755。

按照 8 进制，权限从 0 到 7 刚好对应 perm 数组里面的项。

假如我们想提取一段二进制里面的某一段数据怎么办，做 & 运算，只有为 1 的才会保留。

011 -> 6
&
111 -> 7
||
011 -> 6

st_mode -> 100666 10 代表目录文件 666 表示权限
mask -> 0700 0 表示 8 进制。

做 & 运算，得到 600 ，然后 >> 2 位 得到 6。

mask 做右移 >> 1 则 mask -> 070

做 & 运算，得到 060，然后 >> 1 位，得到 6。

```c
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
```

## 获取用户信息

passwd 中有用户的信息，可以通过 uid，可以通过 name 获取。

```c
#include <stdlib.h>
#include <pwd.h>
#include <stdio.h>

int main(int argc, char const *argv[])
{
    struct passwd *ptr;
    // uid_t uid;
    // uid = atoi(argv[1]);
    // ptr = getpwuid(uid); // 通过 uid 获取
    ptr = getpwnam("yugo");
    printf("%d\n", ptr->pw_uid);
    return 0;
}
```

## 修改文件时间

utime 函数修改时间

```c
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
    printf("actime %s\n", ptr); // 访问时间
    ptr = ctime(&buf.modtime);
    printf("modtime %s\n", ptr); // 修改内容时间
    utime(argv[1], &buf);
    return 0;
}
```

## 实现 ls

对前面的内容进行整合，通过链表实现存储结构。

```c
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <string.h>
#include <fcntl.h>
#include <dirent.h>
#include <pwd.h>
#include <grp.h>

#define NAME_SIZE 32

struct fnode
{
    struct fnode *next; // 指向下一个节点
    char name[NAME_SIZE];
};

void output_type_perm(mode_t mode) // 文件类型，取前 4 位
{
    char type[7] = {'p', 'c', 'd', 'b', '-', 'l', 's'};
    int index = (mode >> 12 & 0xF) / 2; // 存储的都是 2 的倍数
    printf("%c", type[index]);
    static char *perm[] = {"---", "--x", "-w-", "-wx", "r--", "r-x", "rw-", "rwx"};
    printf("%s", perm[mode >> 6 & 07]); // 9 位
    printf("%s", perm[mode >> 3 & 07]); // 6 位
    printf("%s", perm[mode >> 0 & 07]); // 最后 3位
}

void output_user_group(uid_t uid, gid_t gid) // 拿到名字
{
    struct passwd *user;
    user = getpwuid(uid);
    printf(" %s", user->pw_name);
    struct group *group;
    group = getgrgid(gid);
    printf(" %s", group->gr_name);
}

void output_mtime(time_t mytime) // 修改时间输出
{
    char buf[256];
    memset(buf, '\0', 256);
    ctime_r(&mytime, buf);
    buf[strlen(buf) - 1] = '\0';
    printf("  %s", buf);
}

void output_info(struct fnode *head) // 输出当个节点
{
    struct fnode *temp = head;
    while (temp != NULL)
    {
        struct stat file_stat;
        if (-1 == stat(temp->name, &file_stat))
        {
            perror("stat");
            exit(-1);
        }
        output_type_perm(file_stat.st_mode);
        printf("  %4d", file_stat.st_nlink); // 硬链接数 为 0 会自动删除，大多数为1
        output_user_group(file_stat.st_uid, file_stat.st_gid);
        printf("  %8ld", file_stat.st_size);
        output_mtime(file_stat.st_mtime);
        printf("  %s\n", temp->name);
        temp = temp->next;
    }
}

struct fnode *insert_list(struct fnode *current, struct fnode *prev) // 做连接
{
    if (prev == NULL)
        prev = (struct fnode *)malloc(sizeof(struct fnode));
    prev->next = current;
    return current;
}

void free_list(struct fnode *head) // 清空资源
{
    struct fnode *prev = head->next;
    struct fnode *current = head;

    while (prev != NULL)
    {
        free(current);
        current = prev;
        prev = prev->next;
    }
}

int main(int argc, char const *argv[])
{
    int i;
    for (i = 1; i < argc; i++)
    {
        struct fnode *linklist = NULL;
        struct stat file_stat;
        if (stat(argv[i], &file_stat) == -1) // 先拿到文件信息
        {
            perror("stat error");
            return -1;
        }
        if (S_ISREG(file_stat.st_mode)) // 是普通文件
        {
            struct fnode *temp = (struct fnode *)malloc(sizeof(struct fnode));
            if (NULL == temp)
            {
                perror("malloc error");
                return -1;
            }
            temp->next = NULL;
            memset(temp->name, '\0', NAME_SIZE);
            memcpy(temp->name, argv[i], strlen(argv[i]));
            linklist = insert_list(temp, linklist);
            output_info(linklist);
        }
        else if (S_ISDIR(file_stat.st_mode)) // 是目录
        {
            char buf[NAME_SIZE];
            getcwd(buf, 128);
            DIR *dirp = NULL;
            dirp = opendir(argv[i]);
            if (NULL == dirp)
            {
                perror("open dir");
                return -1;
            }
            struct dirent *entp = NULL;
            while (entp = readdir(dirp))
            {
                struct fnode *temp = (struct fnode *)malloc(sizeof(struct fnode));
                if (NULL == temp)
                {
                    perror("malloc fnode error");
                    return -1;
                }
                temp->next = NULL;
                memset(temp->name, '\0', NAME_SIZE); // 生成空间
                memcpy(temp->name, entp->d_name, strlen(entp->d_name)); // 字符串赋值
                linklist = insert_list(temp, linklist);
            }
            chdir(argv[i]);
            close(dirp);
            output_info(linklist);
            chdir(buf);
        }
        free_list(linklist);
    }
    return 0;
}
```

## 守护进程

进程与子进程公用代码段，运行同一份代码，通过 pid 区分是子进程还是父进程。

如何创建后台的守护进程，首先需要忽略一些进程信号。然后两次fork，在第一次fork的时候，通过 setsid() 开启一个新的进程会话，或者说进程组，当前执行进程成为首进程，由于不需要开启终端，再次fork，关闭父进程。

```c
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <sys/param.h>
#include <sys/stat.h>
#include <time.h>
#include <syslog.h>

int init_daemon(void)
{
    int pid;
    int i;

    //忽略终端I/O信号，STOP信号
    signal(SIGTTOU, SIG_IGN); // 忽略 ignore 挂起信号
    signal(SIGTTIN, SIG_IGN); // 忽略后台从 tty 读数据
    signal(SIGTSTP, SIG_IGN); // 忽略终端挂起信号
    signal(SIGHUP, SIG_IGN); // 忽略断开

    pid = fork(); // = 0 代表子进程，< 0 代表出错，父进程返回的值是进程号
    if (pid > 0)
    {
        exit(0); //结束父进程，使得子进程成为后台进程
    }
    else if (pid < 0) // 出错
    {
        return -1;
    }

    //建立一个新的进程组,在这个新的进程组中,子进程成为这个进程组的首进程,以使该进程脱离所有终端
    setsid();

    //再次新建一个子进程，退出父进程，保证该进程不是进程组长，同时让该进程无法再打开一个新的终端
    pid = fork();
    if (pid > 0)
    {
        exit(0);
    }
    else if (pid < 0)
    {
        return -1;
    }

    //关闭所有从父进程继承的不再需要的文件描述符
    for (i = 0; i < NOFILE; close(i++)) // NOFILE 是宏
        ;

    //改变工作目录，使得进程不与任何文件系统联系
    chdir("/");

    //将文件当时创建屏蔽字设置为0
    umask(0);

    //忽略SIGCHLD信号
    signal(SIGCHLD, SIG_IGN); // 忽略子进程信号

    return 0;
}

int main()
{
    time_t now;
    init_daemon();
    syslog(LOG_USER | LOG_INFO, "TestDaemonProcess! \n"); // 写入日志，在/var/log/message 里面可以看到写入的信息。
    while (1)
    {
        sleep(8);
        time(&now);
        syslog(LOG_USER | LOG_INFO, "SystemTime: \t%s\t\t\n", ctime(&now));
    }
}
```
