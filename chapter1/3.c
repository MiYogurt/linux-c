#include <stdio.h>
#include <stdlib.h>

int main(int argc, char const *argv[])
{
    int *arr1 = (int *)calloc(4, sizeof(int));
    int *arr2 = (int *)malloc(4 * sizeof(int));

    for (int i = 0; i < 4; i++)
    {
        printf("%d\t", arr1[i]);
    }

    printf("\n========\n");

    for (int i = 0; i < 4; i++)
    {
        printf("%d\t", arr2[i]);
    }

    return 0;
}
