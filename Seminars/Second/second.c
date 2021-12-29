#include "stdio.h"
#include "stdlib.h"
#include "errno.h"
#include <unistd.h>

#define MAXLEN 256

int main(int argc, char* argv[]){

    printf("file name = %s\n", argv[0]);
    char* new_args[argc + 1];
    char filename[MAXLEN] = "./first";

    new_args[0] = "./first";
    for (int i = 1; i < argc; i++){

        new_args[i] = argv[i];
    }
    new_args[argc] = NULL;

    execvp(argv[1], (argv + 1));    
    //printf("in second execvp = %d\n", execvp(filename, new_args));
}   