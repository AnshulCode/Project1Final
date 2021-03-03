#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
struct stat sb;

void listFiles(const char *path)
{
	struct dirent *dp;
	DIR *dir = opendir(path);

	if (!dir)
		return;

	while ((dp = readdir(dir)) != NULL)
	{
		stat(dp->d_name, &sb);
		if (S_ISREG(sb.st_mode))
			printf("%s\n", dp->d_name);
	}

	closedir(dir);
}

int main(int argc, char* argv[]) {
	if (argc == 2){
		stat(argv[1], &sb);
		if (S_ISREG(sb.st_mode))
			printf("%s is a regular file", argv[1]);
		if (S_ISDIR(sb.st_mode))
			listFiles(argv[1]);
	}
	printf("\n");
	return 0;
}

