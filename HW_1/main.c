#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>

#define MAXLEN 100
#define SIZE_OF_SEGMENT 1000

/*
void input_prog(char* file_name){

    int file = open(file_name, O_RDONLY), str_size = 0, data_has_written = 0, in_progress = -2;
    char check = '!';

    while (data_has_written == 0){

        if (mkfifo("./fifo/connection_pipe", S_IFIFO) == 0){ //открываем для чтения тк эо сигнал того, что процесс жив и при записи не возникнет ошибки

            if (open("./fifo/connection_pipe", O_RDONLY) != -1){

                start_writing(file);
            }
            
        } else{

            in_progress = open("./fifo/connection_pipe", O_WRONLY);

            if (in_progress != -1){

                if (write(in_progress, &check, 0) == -1){
                    
                    unlink("./fifo/connection_pipe");
                } else{

                    close(in_progress); //надо подумать где его закрывать
                }
            }
        } 
    }
}*/

void put_file_to_pipe(int inp_file, int pipe_write_end){

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
}

void get_file_from_pipe(int pipe){

    int read_ret = -1;
    char segment[SIZE_OF_SEGMENT];

    while ((read_ret = read(pipe, segment, SIZE_OF_SEGMENT)) != 0){
                
        write(STDOUT_FILENO, segment, read_ret);
    }
}

void input_prog(int file){

    int got_connection = 0, connection_pipe = open("./fifo/connection_pipe", O_RDONLY) ;
    char byte = '1';

    while (connection_pipe == -1){

        connection_pipe = open("./fifo/connection_pipe", O_RDONLY);
    }

    printf("connection have opened\n");

    got_connection = read(connection_pipe, &byte, 1);

    printf("read smth\n");
    if (got_connection != 1){
        
        if (got_connection == -1){

            printf("dafuq\n");
            exit(1);
        }
        printf("'Write' side of pipe is busy\n");
        exit(0);
    }

    int write_pipe = open("./fifo/main_pipe", O_WRONLY);

    if (write_pipe == -1){

        printf("Processes have connected but the pipe has not been created\n");
        exit(0);
    }

    put_file_to_pipe(file, write_pipe);

    close(write_pipe);
    close(connection_pipe);
}

void output_prog(){

    char byte = '1';
    int connect_fifo = -2, read_pipe = -2;

    if (mkfifo("./fifo/connection_pipe", S_IFIFO | 0777) == -1){ //критическая секция read

        printf("'Read' side of pipe is busy\n");
        exit(0);
    }
    
    if (mkfifo("./fifo/main_pipe", S_IFIFO | 0777) == -1){

        printf("Pipe has alredy been created\n");
        exit(0);
    }

    printf("connection = %i\n", connect_fifo = open("./fifo/connection_pipe", O_WRONLY));

    //раньше read был тут

    if (write(connect_fifo, &byte, 1) == -1){

        printf("Can't write to connection pipe\n");
        exit(0);
    }
    printf("write smth\n");
    close(connect_fifo);

    read_pipe = open("./fifo/main_pipe", O_RDONLY); //вот тут встает 

    get_file_from_pipe(read_pipe);

    close(read_pipe);
}

int open_file(char* file){

    int tmp = open(file, O_RDONLY);

    if (tmp == -1){

        printf("Can't open file\n");
        exit(0);
    }

    return tmp;
}

int main(int argc, char* argv[]){

    if (argc == 1){

        output_prog();
    } else{

        input_prog(open_file(argv[1]));
    }
}