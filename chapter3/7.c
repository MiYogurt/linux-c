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
    struct fnode *next;
    char name[NAME_SIZE];
};

void output_type_perm(mode_t mode)
{
    char type[7] = {'p', 'c', 'd', 'b', '-', 'l', 's'};
    int index = (mode >> 12 & 0xF) / 2; // 存储的都是 2 的倍数
    printf("%c", type[index]);
    static char *perm[] = {"---", "--x", "-w-", "-wx", "r--", "r-x", "rw-", "rwx"};
    printf("%s", perm[mode >> 6 & 07]); // 9 位
    printf("%s", perm[mode >> 3 & 07]); // 6 位
    printf("%s", perm[mode >> 0 & 07]); // 最后 3位
}

void output_user_group(uid_t uid, gid_t gid)
{
    struct passwd *user;
    user = getpwuid(uid);
    printf(" %s", user->pw_name);
    struct group *group;
    group = getgrgid(gid);
    printf(" %s", group->gr_name);
}

void output_mtime(time_t mytime)
{
    char buf[256];
    memset(buf, '\0', 256);
    ctime_r(&mytime, buf);
    buf[strlen(buf) - 1] = '\0';
    printf("  %s", buf);
}

void output_info(struct fnode *head)
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

struct fnode *insert_list(struct fnode *current, struct fnode *prev)
{
    if (prev == NULL)
        prev = (struct fnode *)malloc(sizeof(struct fnode));
    prev->next = current;
    return current;
}

void free_list(struct fnode *head)
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
        if (stat(argv[i], &file_stat) == -1)
        {
            perror("stat error");
            return -1;
        }
        if (S_ISREG(file_stat.st_mode))
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
        else if (S_ISDIR(file_stat.st_mode))
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
                memset(temp->name, '\0', NAME_SIZE);
                memcpy(temp->name, entp->d_name, strlen(entp->d_name));
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
