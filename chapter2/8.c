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
        // fwrite(buf, 1, num, fp_des);
        fwrite(buf, num, 1, fp_des);
        if (feof(fp_src))
            break;
    } while (1);
    fclose(fp_src);
    fclose(fp_des);
    return 0;
}
