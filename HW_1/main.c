#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>

#define MAXLEN 100
#define SIZE_OF_SEGMENT 10

void put_file_to_pipe(int inp_file, int pipe_write_end){

    int end_of_file = 0, size_of_cur_segment = 0;
    char segment[SIZE_OF_SEGMENT];

    while (end_of_file == 0){
        
        size_of_cur_segment = read(inp_file, segment, SIZE_OF_SEGMENT);
        
        if (size_of_cur_segment != -1){
            
            write(pipe_write_end, segment, size_of_cur_segment);
        } else{

            printf("Read error\n");
            exit(1);
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
        
        if (read_ret == -1){

            printf("Problems with pipe\n");
            exit(1);
        }
        write(STDOUT_FILENO, segment, read_ret);
    }
}

void input_prog(int fd){

    mkfifo("./fifo/cur_pipe", 0777);
    mkfifo("./fifo/cur_connection", 0777);

    int connection_pipe = open("./fifo/cur_connection", O_WRONLY);
    if (connection_pipe == -1){

        printf("Other process has deleted connection fifo\n");
        exit(0);
    }

    int write_pipe = open("./fifo/cur_pipe", O_WRONLY);
    if (write_pipe == -1){

        printf("Other process has deleted main fifo\n");
        exit(0);
    }
    //начало критической секции
    int delete_fifo = unlink("./fifo/cur_pipe");
    int delete_connection = unlink("./fifo/cur_connection");

    if (delete_connection != 0){

        close(write_pipe);
        printf("Other process writing to pipe\n");
        exit(0);
    }
    
    write(connection_pipe, "1", 1);//тут уже один процесс
    close(connection_pipe);
    
    //конец критической секции write
    put_file_to_pipe(fd, write_pipe);
}

void output_prog(){

    char byte = '!';
    int connect_pipe = open("./fifo/cur_connection", O_RDONLY);

    while (connect_pipe == -1){
        
        connect_pipe = open("./fifo/cur_connection", O_RDONLY);
    } 

    int read_pipe = open("./fifo/cur_pipe", O_RDONLY);
    if (read_pipe == -1){

        printf("Can't open main pipe\n");
        exit(0);
    }
    //начало критической секции read
    int got_byte = read(connect_pipe, &byte, 1);
    
    if (got_byte != 1){

        printf("Other process reading from pipe\n");
        exit(0);
    }
    
    //конец критической секции read
    get_file_from_pipe(read_pipe);//тут уже один процесс
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