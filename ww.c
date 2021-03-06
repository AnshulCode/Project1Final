#include <fcntl.h> 
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include "strbuf.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <regex.h>

#ifndef DEBUG
#define DEBUG 0
#endif

#ifndef BFSIZE
#define BFSIZE 256
#endif
struct stat sb;

char buff[BFSIZE];
void get_file(const char *path)
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

int word_wrap(int limit, char* name){
	int fd;
	if (name){
		char* fname = name;
		fd = open(fname,O_RDONLY);
	} else {
		fd = 0;
		printf("press enter when complete your input:\n");
	}
	int lim = limit;
	int bytes_read = read(fd,buff,BFSIZE);
	printf("bytes_read: %d", bytes_read);
	putchar('\n');
	printf("%s",buff);
	int i = 0;
	int count_tot = 0;
	int count_word = 0;
	int newline = 0;
	putchar('\n');
	printf("strlen : %lu",strlen(buff) );
	putchar('\n');

	strbuf_t* word = (strbuf_t*)malloc(sizeof(strbuf_t));
	sb_init(word,1);
	// prints word wrapping needs work but base case works
	while(buff[i]!= '\0' && i<strlen(buff)-1){
		/*
		   if(buff[i] == ' '){
		   while(buff[i] == ' ' || buff[i] == '\n'){
		   i++;
		   }
		   }
		   */
		while(buff[i] == ' ') {
			//if(DEBUG) printf("i:%d,", i);
			i++;
		}
		if(buff[i] == '\n') { 
			if(DEBUG) printf("nl:%d ", newline);
			if (DEBUG) printf("(%d,%d)", count_word, count_tot);
			newline = 1;
		}

		while(buff[i] != ' '){
			if(buff[i] == '\n' || buff[i] == '\0'){
				break;
			}
			sb_append(word,buff[i]);
			count_word++;
			//printf(" >>count_word: %d, count_tot: %d\n", count_word, count_tot);
			i++;
		}

		if(count_word>=lim){
			exit(EXIT_FAILURE);
		}

		if (DEBUG) printf("(%d,%d)", count_word, count_tot);
		if (newline) {
			if(DEBUG) printf("<nl>");
			putchar('\n');
			putchar('\n');
			newline = 0;
			count_tot = 0;
			count_word = 0;
		}
		if(count_word+count_tot <lim){
			//if (DEBUG) printf("if:(%d,%d)", count_word, count_tot);
			print_string(word);
			putchar(' ');
			word = (strbuf_t*)malloc(sizeof(strbuf_t));
			sb_init(word,1);
			i++;
			count_tot = count_tot+count_word+1;
			//count_tot++;
			count_word = 0;
		}else{
			//if (DEBUG) printf("else:(%d,%d)", count_word, count_tot);
			putchar('\n');
			print_string(word);
			putchar(' ');
			word = (strbuf_t*)malloc(sizeof(strbuf_t));
			sb_init(word,1);
			i++;
			count_tot = count_word;
			count_word = 0;
		}

	}
	close(fd);
	return 0;
}

int main(int argc, char* argv[]) {
	if (argc < 2 ) {
		printf("USAGE: %s  <page width> (optional)<file or dir name>\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	int width = atoi(argv[1]);
	if ( width < 1) {
		printf("ERROR: line width must be positive\n");
		exit(EXIT_FAILURE);
	}
	if (argc == 2)
		word_wrap(width, NULL);
	if (argc == 3){
		stat(argv[2], &sb);
		if (S_ISREG(sb.st_mode))
			word_wrap(width, argv[2]);
		if (S_ISDIR(sb.st_mode))
			get_file(argv[2]);
	}
	printf("\n");
	return 0;
}




