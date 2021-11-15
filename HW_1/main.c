#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>

#define MAX_NAME_SIZE 10
#define SIZE_OF_SEGMENT 10
#define CONNECT_PIPE_NAME "./fifo/connection"

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

char* generate_name(char* buf, pid_t pid){
    int i = 0;

    for (i = 0; pid != 0; i++){

        buf[i] = '0' + (pid % 10);
        pid = pid / 10;
    }
    buf[i] = '\0';
    return buf;
}


void consumer(){

    char buf[MAX_NAME_SIZE];
    pid_t cur_pid = getpid();
    char* unique_fifo_name = generate_name(buf, cur_pid);

    mkfifo(CONNECT_PIPE_NAME, 0777);

    if (mkfifo(unique_fifo_name, 0777) == -1){

        printf("Can't create unique fifo\n");
        exit(0);
    }

    int connection_pipe = open(CONNECT_PIPE_NAME, O_WRONLY);
    if (connection_pipe == -1){

        printf("Can't open connection fifo\n");
        exit(0);
    }

    int tmp_RW = open(unique_fifo_name, O_RDWR);
    if (tmp_RW == -1){

        printf("Unique fifo doesn't exist\n");
        exit(0);
    }

    int unique_read_pipe = open(unique_fifo_name, O_RDONLY);
    if (unique_read_pipe == -1){

        printf("Can't open unique fifo\n");
        exit(0);
    }
    close(tmp_RW);

    if (write(connection_pipe, &cur_pid, sizeof(pid_t)) == -1){

        printf("Can't write to connection pipe\n");
        exit(0);
    }
    close(connection_pipe);

    sleep(2);
    
    get_file_from_pipe(unique_read_pipe);
    unlink(unique_fifo_name);
}

void producer(int fd){

    pid_t cons_id = 0;

    mkfifo(CONNECT_PIPE_NAME, 0777);

    int connection_pipe = open(CONNECT_PIPE_NAME, O_RDONLY);
    if (connection_pipe == -1){

        printf("Can't open connection fifo\n");
        exit(0);
    }

    if (read(connection_pipe, &cons_id, sizeof(pid_t)) <= 0){

        printf("Can't read consumer id\n");
        exit(0);
    }
    close(connection_pipe);

    char buf[MAX_NAME_SIZE];
    char* unique_fifo_name = generate_name(buf, cons_id);

    int tmp_RW = open(unique_fifo_name, O_RDWR);
    if (tmp_RW == -1){

        printf("Unique fifo doesn't exist\n");
        exit(0);
    }

    int unique_write_fifo = open(unique_fifo_name, O_WRONLY);
    if (unique_write_fifo == -1){

        printf("Can't open write end of unique fifo\n");
        exit(0);
    }
    close(tmp_RW);

    put_file_to_pipe(fd, unique_write_fifo);
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

        consumer();
    } else{

        producer(open_file(argv[1]));
    }
}