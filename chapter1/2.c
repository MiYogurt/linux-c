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
