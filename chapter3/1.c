#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
// #include <direct.h>

int main(int argc, char const *argv[])
{
	DIR *dirp;
	struct dirent *dp;
	dirp = opendir(argv[1]);
	while ((dp = readdir(dirp)) != NULL)
	{
		printf("name %s \n", dp->d_name);
	}
	closedir(dirp);
	return 0;
}
