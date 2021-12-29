#include "stdio.h"
#include "stdlib.h"
#include "errno.h"


void below_zero(char* str){

    void* end_of_line = NULL;
    int n = (-1) * strtol(str, end_of_line, 10);
     
    for (long i = 1; i >= n; i--){

        printf("%li ", i);
    }
}

void above_zero(char* str){

    void* end_of_line = NULL;
    long n = strtol(str, end_of_line, 10);

    for (long i = 1; i <= n; i++){

        printf("%li ", i);
    }
}

int main(int argc, char* argv[]){

    if (argc >= 2){

        char* end_of_line = NULL;
        strtol(argv[1], &end_of_line, 10);

        if ((errno == EINVAL) || (errno == ERANGE) || (*end_of_line != '\0')){

            printf("Input error\n");  

        } else{
            
            if ((argv[1][0] == '-') || (argv[1][0] == '0')){

                below_zero(&argv[1][1]);
            } else{

                above_zero(argv[1]);
            } 
        }
    }

    printf("\n");
}
