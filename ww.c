
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
#include <errno.h>
#include <stdlib.h>
#define BUFF_LENGTH (256)
#define BYTES_READ (256)
#define DEBUG (0)
struct stat sb;
char buff[BUFF_LENGTH];
char fbuff[BUFF_LENGTH];
char wbuff[BUFF_LENGTH];

void debug(char* title,char* messsage){
	if(DEBUG == 1){
		printf("DEBUG: %s %s\n ",title, messsage);
	}
}
int is_dir(const char *path)
{
    struct stat path_stat;
    stat(path, &path_stat);
    return S_ISDIR(path_stat.st_mode);
}
int word_wrap_dir(int limit, char* filenamep,char* fnamew){
     

	char* fname = filenamep;
	int lim = limit;
	int fd;
	int size = BYTES_READ;
	int bytes_read;
	if(fname){
		debug("Opening file " ,fname);
		fd = open(fname,O_RDONLY);
		if(fd == -1){
			printf("File open error : %d \n" , errno);
			return EXIT_FAILURE;
		}
		bytes_read = read(fd,buff,size);
	}else{
		fd = 0; 
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
	int exit_status = 0;

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
             while(isspace(buff[i])!= 0){
                if(buff[i] == '\n'){
                   count++;
                   if(count == 2){
                     sb_append(new_buf,'\n');
                     sb_append(new_buf,'\n');
                     sizer = sizer+2;
				   }
                }
                i++;
             }
            //`AQprint_string(word);
             word = (strbuf_t*)malloc(sizeof(strbuf_t));
             sb_init(word,1);
             if(count>=2){
                count_tot = 0;
             }
         }
         
		if(count_word>lim){
			 sb_append(new_buf,'\n');
			 sizer++;
			 int ind = 0;
				while(word->data[ind]!='\0'){
				sb_append(new_buf,word->data[ind]);
				ind++;
			}
			sb_append(new_buf,'\n');
			sizer++;
			count_word = 0;
			count_tot = 0;
			exit_status = 1;
			i++;
		}
		if(count_word+count_tot <=lim && count_word != 0){
			int ind = 0;
			while(word->data[ind]!='\0'){
				sb_append(new_buf,word->data[ind]);
				ind++;
			}
			sb_append(new_buf,' ');
			sizer++;
			word = (strbuf_t*)malloc(sizeof(strbuf_t));
			sb_init(word,1);
			if(buff[i] == '\n'){
				i++;
				int count = 1;
				
				while(isspace(buff[i])!= 0){
					if(buff[i] == '\n'){
						count++;
					}
					if(count == 2){
						sb_append(new_buf,'\n');
                    	 sb_append(new_buf,'\n');
                     	sizer = sizer+2;
					}
					i++;
				}
				if(count>=2){
					count_tot = 0;
					count_word = 0;
				}
			}else{
				i++;
				count_tot = count_tot+count_word+1;
				count_word = 0;
			}
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
	
	int fdw = open(fnamew, O_RDWR | O_CREAT, S_IWUSR | S_IRUSR);
	write(fdw,write_data,sizer);
	close(fdw);
	if(exit_status == 1){
		printf("%c",'\n');
		perror("words are too long");
		close(fdw);
		close(fd);
		return EXIT_FAILURE;
	}
	close(fd);
	return EXIT_SUCCESS;
}


int word_wrap(int limit, char* filename){

	char* fname = filename;
	int lim = limit;
	int fd;
	int size = BYTES_READ;
	int bytes_read;
	if(fname){
		printf("Opening file : %s \n" ,fname);

		fd = open(fname,O_RDONLY);
		if(fd == -1){
			printf("File open error : %d \n" , errno);
			return EXIT_FAILURE;
		}
		if(fd == 2){
			printf("File open error : %d \n" , errno);
			return EXIT_FAILURE;
		}
		bytes_read = read(fd,buff,size);
	}else{
		fd =  0; 
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
	int exit_status = 0;
	

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
             while(isspace(buff[i])!= 0){
                if(buff[i] == '\n'){
                   count++;
                   if(count == 2){
                     sb_append(new_buf,'\n');
                     sb_append(new_buf,'\n');
                     sizer = sizer+2;
				   }
                }
                i++;
             }
            //`AQprint_string(word);
             word = (strbuf_t*)malloc(sizeof(strbuf_t));
             sb_init(word,1);
             if(count>=2){
                count_tot = 0;
             }
         }
         
		if(count_word>lim){
			 sb_append(new_buf,'\n');
			 sizer++;
			 int ind = 0;
				while(word->data[ind]!='\0'){
				sb_append(new_buf,word->data[ind]);
				ind++;
			}
			sb_append(new_buf,'\n');
			sizer++;
			count_word = 0;
			count_tot = 0;
			exit_status = 1;
			i++;
		}
		if(count_word+count_tot < lim && count_word != 0){
			int ind = 0;
			while(word->data[ind]!='\0'){
				sb_append(new_buf,word->data[ind]);
				ind++;
			}
			sb_append(new_buf,' ');
			sizer++;
			word = (strbuf_t*)malloc(sizeof(strbuf_t));
			sb_init(word,1);
	      if(buff[i] == '\n'){
				i++;
				int count = 1;
				
				while(isspace(buff[i])!= 0){
					if(buff[i] == '\n'){
						count++;
					}
					if(count == 2){
						sb_append(new_buf,'\n');
                    	 sb_append(new_buf,'\n');
                     	sizer = sizer+2;
					}
					i++;
				}
				if(count>=2){
					count_tot = 0;
					count_word = 0;
				}
			}else{
				i++;
				count_tot = count_tot+count_word+1;
				count_word = 0;
			}
			
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
		      if(buff[i] == '\n'){
				i++;
				int count = 1;
				
				while(isspace(buff[i])!= 0){
					if(buff[i] == '\n'){
						count++;
					}
					if(count == 2){
						sb_append(new_buf,'\n');
                    	 sb_append(new_buf,'\n');
                     	sizer = sizer+2;
					}
					i++;
				}
				if(count>=2){
					count_tot = 0;
					count_word = 0;
				}
			}
			sb_init(word,1);
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
	if(exit_status == 1){
		printf("%c",'\n');
		perror("words are too long");
		return EXIT_FAILURE;
	}
	close(fd);

	return EXIT_SUCCESS;
}
void create_write_file_name(char* f, char* mainb){
   char* filen = f;
  strcat(mainb,"wrap.");
  int i = 0;
  int count=0;
  char* plc = filen;  
  while((plc[i]>=32&& plc[i]<=127)|| i< strlen(filen)-1){
        if(plc[i] == '.'){
            break;
        }
        count++;
        i++;
  }
  char* add = (char*)malloc(sizeof(char)*count);
  for(int j = 0; j<count; j++){
      add[j] = filen[j]; 
  }
  strcat(mainb, add);
}

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
		rc = regcomp(&regex, pattern, 0);
                rc = regexec(&regex, dp->d_name, 0, NULL, 0);
                debug("opening filename", dp->d_name);
				// debug("opening rc", itoa(rc));
                if (dp->d_name[0] != '.' && rc){
                    debug("filename ",dp->d_name);
				    char *f = fbuff;
				    char *w = wbuff;
					strcat(fbuff,path);
					strcat(wbuff,path);
				   debug("before concat r",f);
				   debug("before concat w",w);
				   strcat(w,"/");
				   strcat(f,"/");
				   strcat(f,dp->d_name);
				   create_write_file_name(dp->d_name,w);
				   debug("read file path",f);
				   debug("write file path",w);
				   if(is_dir(f)!=0){
					 debug("is dir",f);
				   }else{
				   	word_wrap_dir(8,f,w);
				   }
				   strcpy(f, "");
				   strcpy(w, "");
				}
	}

	closedir(dir);
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
	
    
   
   
	return 0;
}

