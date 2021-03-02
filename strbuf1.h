#include <stdlib.h>
#include <stdio.h>
typedef struct {
    size_t length;
    size_t used;
    char *data;
} strbuf_t;

int sb_init(strbuf_t* L , size_t length){
    L->data = malloc(sizeof(char)*(length+1));
    if (!L->data) return 1;

    L->length = length+1;
    L->data[0] = '\0';
    L->used=1;
    return 0;
}
void sb_destroy(strbuf_t *L)
{
    free(L->data);
}
int sb_append(strbuf_t *L,char item){
    if(L->length == L->used){
        int size =  (L->length*2)-1;
        char* p = (char*)realloc(L->data,sizeof(char)*size);
        if(!p){
             return 1;
        }
        p[size-1] = '\0';
        L->data = p;
        L->length = size;    
    } 
       L->data[L->used-1] = item;
       ++L->used;
     return 0;
}
int sb_insert(strbuf_t *L,int index,char item){
       if(L->length == L->used){
        int size =  (L->length*2)-1;
        char* p = (char*)realloc(L->data,sizeof(char)*size);
        if(!p){
             return 1;
        }
        p[size-1] = '\0';
       /* int i = 0;
            while(i<size-1){
                if(p[i] == '\0'){
                    p[i]= ' ';
                }
                i++;
            }
            */
        L->data = p;
        L->length = size;    
    }
    if(index+1<=L->length-1){
        char* insert = (char*)malloc(sizeof(char)*(L->length+1));
        int i = 0;
        for(i = 0; i<index;i++){
            insert[i] = L->data[i];
        } 
        insert[i] = item;
        for(i = i+1; i<L->length+1;i++){
            insert[i] = L->data[i-1];
        }
        L->data = insert;

        L->length = L->length+1;
        ++L->used;
        return 0;
    }else{
        if(index<L->length*2-1){
            int size =  (L->length*2)-1;
            char* p = (char*)realloc(L->data,sizeof(char)*size);
            p[size-1] = '\0';
            p[index] = item;
        
            
            L->data = p;
            L->length = size;
            ++L->used;
            return 0;
        }else{
            size_t size = index+2;
	        int *p = realloc(L->data, sizeof(char)*size);
	        if (!p) return 1;
	        L->data = (char *)p;
            L->data[size-1] = '\0';
            L->data[index] = item;
	        L->length = size;
            ++L->used;
	    return 0;
        }
    }
}
void sb_status(strbuf_t *L){

    printf("Used :%lu\n",L->used);
    printf("Length: %lu\n",L->length);
    int i = 0;
    for(i = 0; i<L->length-1;i++){
     
         if(L->data[i]>=32 && L->data[i]<=127){
             printf("%c",L->data[i]);
         }else{
              printf("|");
         }
         
        
    }
    if(!(L->data[i]>=32 && L->data[i]<=127)){
        printf("~");
    }
}
int sb_concat(strbuf_t *sb, char *str){
    if(sb->used == sb->length){
        int i = 0;
        while(str[i]!= '\0'){
            sb_append(sb,str[i]);
            i++;
            ++sb->used;
        }
        return 0;
    }else{
        int i =sb->length-2;
        int count = 0;
        while(str[count]!= '\0'){
            sb_insert(sb,i+count,str[count]);
            count++;
        }
        return 0;
    }
    return 0;
}
void print_string(strbuf_t *sb){
    printf("%s",sb->data);
}
