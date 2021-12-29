#include "stdio.h"
#include "stdlib.h"
#include "errno.h"
#include <unistd.h>
#include <fcntl.h>

#define SIZE_OF_SEGMENT 1000

void print_to_pipe(int inp_file, int pipe_write_end){

    int end_of_file = 0, size_of_cur_segment = 0;
    char segment[SIZE_OF_SEGMENT];

    while (end_of_file == 0){
        
        size_of_cur_segment = read(inp_file, segment, SIZE_OF_SEGMENT);
        
        if (size_of_cur_segment != -1){

            write(pipe_write_end, segment, size_of_cur_segment);
        } else{

            printf("Read error\n");
        }

        if (size_of_cur_segment != SIZE_OF_SEGMENT){

            end_of_file = 1;
        }
    }

    close(inp_file);
    close(pipe_write_end);
}


int main(int argc, char* argv[]){

    pid_t child_id = -1;
    char c = '!';
    int pipe_ret = 0;
    int pipefd[2];
    pipefd[0] = -1;
    pipefd[1] = -1;

    if (argc >= 2){

        int inp_file = open(argv[1], O_RDONLY);

        if (inp_file == -1){

            printf("Cant open file\n");
            
        } else{

            pipe_ret = pipe(pipefd);

            child_id = fork();

            if (child_id == 0){

                print_to_pipe(inp_file, pipefd[1]);
            } else{

                while (read(pipefd[0], &c, sizeof(char) != 0)){

                    write(stdout, &c, sizeof(char));
                }
            }
        }
    } else{

        printf("Few arguments\n");
    }
}