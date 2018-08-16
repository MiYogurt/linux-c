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

    struct dirent *entp = NULL;

    while (NULL != (entp = readdir(dirp)))
    {
        if (strcmp(entp->d_name, "..") == 0 || strcmp(entp->d_name, ".") == 0)
        {
            continue;
        }

        char *name_src = (char *)malloc(strlen(src) + strlen(entp->d_name) + 2);
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

        else if (S_ISDIR(stat_src.st_mode))

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

    umask(0000);

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
                if (op == 'y' || op == 'Y')
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
