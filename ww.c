
#include <fcntl.h> 
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include "strbuf.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <regex.h>
#include <ctype.h>
#define BUFF_LENGTH (256)
#define BYTES_READ (256)
struct stat sb;
char buff[BUFF_LENGTH];

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
		if (S_ISREG(sb.st_mode))
			printf("%s\n", dp->d_name);
	}

	closedir(dir);
}

int word_wrap(int limit, char* filename){

	char* fname = filename;
	int lim = limit;
	int fd;
	int size = BYTES_READ;
	int bytes_read;
	if(fname){
		fd = open(fname,O_RDONLY);
		bytes_read = read(fd,buff,size);
	}else{
		fd = open(0,O_RDONLY); 
		bytes_read = read(0,buff,size);
	}

	int str = strlen(buff); 
	if(bytes_read ==  str && fd !=0){
		while(bytes_read ==  str){
			close(fd);
			size = size*2;
			fd = open(fname,O_RDONLY);
			read(fd,buff,size);
			bytes_read = read(fd,buff,size);
			str = strlen(buff); 
		}
	}

	int i = 0;
	int count_tot = 0;
	int count_word = 0;
	int sizer = 0;

	strbuf_t* word = (strbuf_t*)malloc(sizeof(strbuf_t));
	sb_init(word,1);
	strbuf_t* new_buf = (strbuf_t*)malloc(sizeof(strbuf_t));
	sb_init(new_buf,size);

	// prints word wrapping needs work but base case works
	while(buff[i]!= '\0' && i<strlen(buff)-1){
		if(isspace(buff[i])!=0&& buff[i]!='\n'){
			while(isspace(buff[i])!=0){
				i++;
			}
		}
		while(isspace(buff[i])==0){
			if(buff[i] == '\n' ||buff[i] == '\0'){
				break;
			}
			sizer++;
			sb_append(word,buff[i]);
			count_word++;
			i++;
		}
		if(buff[i] == '\n' && count_word == 0){
			int count = 0;
			//sb_append(new_buf,'\n');
			while(isspace(buff[i])!= 0){
				if(buff[i] == '\n'){
					count++;
					if(count == 2){
						sb_append(new_buf,'\n');
						sb_append(new_buf,'\n');
						sizer = sizer+2;
					}else if(count >2){
						sb_append(new_buf,'\n');
					}
				}
				i++;
			}
			//`AQprint_string(word);
			word = (strbuf_t*)malloc(sizeof(strbuf_t));
			sb_init(word,1);
			count_tot = 0;
		}

		if(count_word>=lim){
			return EXIT_FAILURE;
		}
		if(count_word+count_tot <lim && count_word != 0){

			int ind = 0;
			while(word->data[ind]!='\0'){
				sb_append(new_buf,word->data[ind]);
				ind++;
			}
			sb_append(new_buf,' ');
			sizer++;
			word = (strbuf_t*)malloc(sizeof(strbuf_t));
			sb_init(word,1);
			i++;
			count_tot = count_tot+count_word+1;
			//count_tot++;
			count_word = 0;
		}else if(count_word != 0){
			sb_append(new_buf,'\n');

			sizer++;
			int ind = 0;
			while(word->data[ind]!='\0'){
				sb_append(new_buf,word->data[ind]);
				ind++;
			}
			sb_append(new_buf,' ');
			sizer++;
			word = (strbuf_t*)malloc(sizeof(strbuf_t));
			sb_init(word,1);
			i++;
			count_tot = count_word;
			count_word = 0;
		}

	}

	sb_append(new_buf,'\n');
	sizer++;

	char* write_data = (char*)malloc(sizeof(char)*sizer);
	int fin = 0;
	while(fin<sizer){
		write_data[fin] = new_buf->data[fin];
		fin++;
	}

	write_data = (char*)realloc(write_data,sizeof(char)*(new_buf->used));
	free(new_buf);
	write(1,write_data,sizer);
	close(fd);

	return EXIT_SUCCESS;
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
	if (argc == 2){
		printf("-- Press 'enter' when you complete your input -- \n");
		word_wrap(width, NULL);
	}
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

