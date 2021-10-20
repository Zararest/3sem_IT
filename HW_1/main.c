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

void input_prog(int file){

    int got_connection = 0, connection_pipe = open("./fifo/from_read", O_RDONLY | O_NONBLOCK) ;
    char byte = '1';

    while (connection_pipe == -1){

        connection_pipe = open("./fifo/from_read", O_RDONLY | O_NONBLOCK);     
    }
    sleep(1);//ждем пока программа read запишет байт в fifo
    
    got_connection = read(connection_pipe, &byte, 1);
    
    if (got_connection != 1){
        
        if (got_connection == -1){

            printf("Can't read from connection pipe\n");
            exit(-1);
        }
        printf("'Write' side of pipe is busy\n");
        exit(0);
    }
    //тут работает один процесс
    int from_write = open("./fifo/from_write", O_WRONLY);
    
    write(from_write, &byte, 1);

    int write_pipe = open("./fifo/main_pipe", O_WRONLY); //блокируется пока не откроется

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
    int from_read = -2, read_pipe = -2, from_write = -2, read_ret = 0;

    mkfifo("./fifo/from_read", 0777);
    mkfifo("./fifo/main_pipe", 0777);
    mkfifo("./fifo/from_write", 0777);

    from_read = open("./fifo/from_read", O_WRONLY);

    if (write(from_read, &byte, 1) == -1){

        printf("Can't write to connection pipe\n");
        exit(0);
    }

    from_write = open("./fifo/from_write", O_RDONLY); 
    
    read_ret = read(read_pipe, &byte, 1);
    
    printf("after_ret\n");
    if (read_ret != 1){

        printf("Can't connect back\n");
        exit(0);
    }

    open("./fifo/main_pipe", O_RDONLY);

    get_file_from_pipe(read_pipe);

    close(read_pipe);
    close(from_write);
    close(from_read);
    unlink("./fifo/from_read");
    unlink("./fifo/main_pipe");
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