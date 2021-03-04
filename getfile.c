#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <regex.h>
struct stat sb;

void listFiles(const char *path)
{
	const char *pattern = "^wrap\\..*";
	regex_t regex;
	int rc;


	struct dirent *dp;
	DIR *dir = opendir(path);

	if (!dir)
		return;

	while ((dp = readdir(dir)) != NULL)
	{
		stat(dp->d_name, &sb);
		rc = regcomp(&regex, pattern, 0);
		rc = regexec(&regex, dp->d_name, 0, NULL, 0);
		//printf("name: %s, rc = %d\n", dp->d_name, rc);
		if (S_ISREG(sb.st_mode) && dp->d_name[0] != '.' && rc)
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
	//printf("\n");
	return 0;
}

