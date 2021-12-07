#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdio.h>
#include <sys/select.h>
#include <math.h>

#define CHECK(expr)                                     \
    do {                                                \
        if (!expr) {                                    \
            printf ("Error in line: %d\n", __LINE__);   \
            exit(0);                                    \
        }                                               \
    } while (0) 

#define CONST_BUF_SIZE 1024

struct fd_for_child{

    int to_child_pipe[2];
    int from_child_pipe[2];
};

struct connection{

    void* buf;
    int max_buf_size;
    int cur_buf_size;
    int bytes_read;
    int bytes_write;
};

int calc_buf_size(int i, int n){

    if (CONST_BUF_SIZE > pow(3, n - i + 4)){

        return CONST_BUF_SIZE;
    } else{

        return pow(3, n - i + 4);
    }
}

void child(int number, int read_fd, int write_fd){

}

void parent(int fd, struct fd_for_childs* arr_of_pipes, int n){

    struct connection* arr_of_con = (struct connection*) calloc(n, sizeof(struct connection)); 

    for (int i = 0; i < n; i++){

        arr_of_con[i].buf = calloc(calc_buf_size(i, n), sizeof(char));
        arr_of_con[i].max_buf_size = calc_buf_size(i, n);
    }




    for (int i = 0; i < n; i++){

        free(arr_of_con[i].buf);
    }

    free(arr_of_con);
}

int main(int argc, char* argv[]){

    CHECK(argc >= 3);

    char* end_of_line = NULL;
    int n = strtol(argv[1], &end_of_line, 10);
    CHECK(end_of_line == '\0');

    struct fd_for_child* arr_of_pipes = (struct fd_for_child*) calloc(n, sizeof(struct fd_for_child));

    int fd = open(argv[2], O_RDONLY);
    CHECK(fd != -1);

    pid_t fir_child_id = fork();
    pipe(arr_of_pipes[0].from_child_pipe);

    arr_of_pipes[0].to_child_pipe[0] = -1;
    arr_of_pipes[0].to_child_pipe[1] = -1;

    if (fir_child_id == 0){

        close(arr_of_pipes[0].from_child_pipe[0]);
        child(0, fd, arr_of_pipes[0].from_child_pipe[1]);
    } else{

        close(arr_of_pipes[0].from_child_pipe[1]);
    }

    for (int i = 2; i < (n - 1); i++){

        pid_t child_id = fork();
        pipe(arr_of_pipes[i].to_child_pipe);
        pipe(arr_of_pipes[i].from_child_pipe);
        printf("buf size = %i\n", calc_buf_size(i, n));

        if (child_id == 0){
            
            close(arr_of_pipes[i].to_child_pipe[1]);
            close(arr_of_pipes[i].from_child_pipe[0]);
            
            child(i, arr_of_pipes[i].to_child_pipe[0], arr_of_pipes[i].from_child_pipe[1]);

            exit(0);
        } else{

            close(arr_of_pipes[i].to_child_pipe[0]);
            close(arr_of_pipes[i].from_child_pipe[1]);
        }
    }

    

    parent(fd, arr_of_pipes, n);

    free(arr_of_pipes);
}