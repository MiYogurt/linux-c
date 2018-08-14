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
