#include "stdio.h"
#include "stdlib.h"
#include "errno.h"
#include <unistd.h>

void create_proc(char* str){

    void* end_of_line = NULL;
    long n = strtol(str, end_of_line, 10);

    if (n <= 0){

        printf("Input error\n");
        exit(0);
    }

    for (int i = 0; i < n; i++){
        
    }
    
}

int main(int argc, char* argv[]){

    if (argc >= 2){

        char* end_of_line = NULL;
        strtol(argv[1], &end_of_line, 10);

        if ((errno == EINVAL) || (errno == ERANGE) || (*end_of_line != '\0')){

            printf("Input error\n");  

        } else{
            
            create_proc(argv[1]);
        }
    } else{

        printf("Not enogh args\n");
    }

    printf("\n");
}