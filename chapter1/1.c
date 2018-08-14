#include <stdio.h>
#include <stdlib.h>

int main(int argc, char const *argv[])
{
    int *array;

    if ((array = (int *)malloc(10 * sizeof(int))) == NULL)
    {
        printf("malloc memory unsuccessful");
        exit(1);
    }

    for (int i = 9; i >= 0; i--)
    {
        array[i] = i;
    }

    for (int i = 9; i >= 0; i--)
    {
        printf("%d \n", array[i]);
    }

    free(array);
    array = NULL;
    return 0;
}
