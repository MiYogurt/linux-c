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
