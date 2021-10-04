#include "stdio.h"
#include "stdlib.h"
#include "errno.h"
#include <unistd.h>

void create_child(int cur_child, int total_number){

    if (cur_child < total_number){

        pid_t new_child = fork();

        if (new_child == 0){

            printf("Я ребенок номер [%d] мой ID[%d](родитель[%d])\n", cur_child, getpid(), getppid());
        } else{

            create_child(cur_child + 1, total_number);
        }
    }
}

void create_proc(char* str){

    void* end_of_line = NULL;
    long n = strtol(str, end_of_line, 10);

    if (n > 0){

        create_child(0, n);
    } else{

        printf("Input error\n");
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
