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