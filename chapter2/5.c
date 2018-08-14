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
