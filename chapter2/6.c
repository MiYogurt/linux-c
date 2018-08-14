#include <stdio.h>
#include <stdlib.h>

typedef struct _stuent
{
    char name[10];
    int number;
} student;

void reader(void)
{
    FILE *f2 = fopen("studen", "r");
    student *s2 = malloc(sizeof(student));
    fread(s2, sizeof(student), 1, f2);
    fclose(f2);
    printf("%s %d\t", s2->name, s2->number);
}

int main(int argc, char const *argv[])
{
    FILE *f1 = fopen("studen", "w");
    if (f1 == NULL)
    {
        perror("open error");
        return -1;
    }
    student s1 = {"Yugo", 13};
    fwrite(&s1, sizeof(student), 1, f1);
    fclose(f1);
    reader();
    return 0;
}
