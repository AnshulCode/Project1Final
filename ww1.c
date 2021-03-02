#include <fcntl.h> 
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include "strbuf1.h"

#ifndef DEBUG
#define DEBUG 0
#endif

#ifndef BFSIZE
#define BFSIZE 256
#endif

char buff[BFSIZE];
int word_wrap(int limit, char* filename){

	char* fname = filename;
	int lim = limit;
	int fd = open(fname,O_RDONLY);
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
			if(DEBUG) printf("i:%d,", i);
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
			if (DEBUG) printf("if:(%d,%d)", count_word, count_tot);
			print_string(word);
			putchar(' ');
			word = (strbuf_t*)malloc(sizeof(strbuf_t));
			sb_init(word,1);
			i++;
			count_tot = count_tot+count_word+1;
			//count_tot++;
			count_word = 0;
		}else{
			if (DEBUG) printf("else:(%d,%d)", count_word, count_tot);
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
	if (argc != 3) {
		printf("USAGE: %s  <page width> <input filename>\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	int width = atoi(argv[1]);
	if ( width < 1) {
		printf("ERROR: line width must be positive\n");
		exit(EXIT_FAILURE);
	}
	if (access(argv[2],F_OK)) {
		printf("ERROR: %s does not exist\n", argv[2]);
		exit(EXIT_FAILURE);
	}

	word_wrap(width, argv[2]);
	printf("\n");
	return 0;
}




