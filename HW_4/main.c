#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdio.h>
#include <sys/select.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

#define SELECT_WAIT 10
#define CHILD_BUF_SIZE 256
#define CONST_BUF_SIZE 1024
#define DEBUG_MODE 1

#define CHECK(expr)                                                                 \
    do {                                                                            \
        if (!(expr)) {                                                              \
            fprintf (stderr, "Error in line: %d. Errno = %i\n", __LINE__, errno);   \
            exit(0);                                                                \
        }                                                                           \
    } while (0) 

#define CLOSE_USELESS_PIPES(cur_child)                      \
    do {                                                    \
        for (int j = 0; j < (n - 1); j++){                  \
                                                            \
            if (j != cur_child){                            \
                                                            \
                close(arr_of_pipes[j].to_child_pipe[0]);    \
                close(arr_of_pipes[j].from_child_pipe[1]);  \
            }                                               \
            close(arr_of_pipes[j].to_child_pipe[1]);        \
            close(arr_of_pipes[j].from_child_pipe[0]);      \
        }                                                   \
    } while (0)

#define CHECK_MAX_FD(num)   \
    do{                     \
        if (num > max_fd){  \
                            \
            max_fd = num;   \
        }                   \
    } while (0)

#define DEBUG(code)             \
    do{                         \
        if (DEBUG_MODE){code;}  \
    } while(0);                      

#define DUMP_CONNECTION(con, num) \
    fprintf(stderr, "Connection[%i]: cur_size{%i} read{%i} write{%i} read_closed{%i}\n", num, con.cur_buf_size, con.bytes_read, con.bytes_write, con.read_fd_is_closed);


struct fd_for_child{

    int to_child_pipe[2];
    int from_child_pipe[2];
};

struct connection{

    char* buf;
    int max_buf_size;
    int cur_buf_size;
    int cur_write_pos;

    int read_fd_is_closed;

    int read_fd;
    int write_fd;
};

int calc_buf_size(int i, int n){
    
    int pow_val = pow(3, n - i + 4);

    if (CONST_BUF_SIZE > pow_val){

        return CONST_BUF_SIZE;
    } else{
        
        return pow_val;
    }
}

void child(int number, int read_fd, int write_fd){

    char child_buf[CHILD_BUF_SIZE];
    int read_ret = -1;
    
    while ((read_ret = read(read_fd, child_buf, CHILD_BUF_SIZE)) != 0){
        
        if (read_ret == -1){

            printf("Problems with pipe in child %i\n", number);
            exit(0);
        }
    
        write(write_fd, child_buf, read_ret);
    }

    close(read_fd);
    close(write_fd);
    exit(0);
}

int transfer_data(struct connection* cur_con, int read_ready, int write_ready, int num){

    int read_ret_val = -1, write_ret_val = -1;
    DEBUG(CHECK(cur_con->cur_buf_size >= 0));
    DEBUG(CHECK(cur_con->cur_buf_size <= cur_con->max_buf_size));
    
    if (cur_con->cur_buf_size != 0){

        if (write_ready != 0){
            
            write_ret_val = write(cur_con->write_fd, cur_con->buf + cur_con->cur_write_pos, cur_con->cur_buf_size);
            CHECK(write_ret_val > 0);
            cur_con->cur_write_pos += write_ret_val;
            cur_con->cur_buf_size -= write_ret_val;
        }
    } else{

        if (read_ready != 0){

            read_ret_val = read(cur_con->read_fd, cur_con->buf, cur_con->max_buf_size);
            CHECK(read_ret_val != -1);
            cur_con->cur_write_pos = 0;
            cur_con->cur_buf_size += read_ret_val;

            if (write_ready != 0){
        
                write_ret_val = write(cur_con->write_fd, cur_con->buf, cur_con->cur_buf_size);
                CHECK(write_ret_val != -1);
                cur_con->cur_buf_size -= write_ret_val;
            }
        }
    } 

    if (read_ret_val == 0){
        
        close(cur_con->read_fd);
        cur_con->read_fd_is_closed = 1;
    }

    return 0;
}

void parent(int fd, struct fd_for_child* arr_of_pipes, int n){
    
    struct connection* arr_of_con = (struct connection*) calloc(n - 1, sizeof(struct connection)); 

    fd_set read_set, write_set;
    struct timeval tv = {SELECT_WAIT, 0};

    int max_fd = 0, read_ready = 0, write_ready = 0, transfer_ret = 0;
    int num_of_closed_con = 0;

    FD_ZERO(&read_set);
    FD_ZERO(&write_set);

    for (int i = 0; i < (n - 1); i++){

        arr_of_con[i].buf = calloc(calc_buf_size(i, n), sizeof(char));
        arr_of_con[i].max_buf_size = calc_buf_size(i, n);

        arr_of_con[i].read_fd = arr_of_pipes[i].from_child_pipe[0];
        arr_of_con[i].write_fd = arr_of_pipes[i + 1].to_child_pipe[1];  

        FD_SET(arr_of_con[i].write_fd, &write_set);
        FD_SET(arr_of_con[i].read_fd, &read_set);

        CHECK_MAX_FD(arr_of_con[i].write_fd);
        CHECK_MAX_FD(arr_of_con[i].read_fd);
    }
    
    while (num_of_closed_con < (n - 1)){
    
        select(max_fd + 1, &read_set, &write_set, NULL, &tv);

        for (int i = num_of_closed_con; i < (n - 1); i++){
            
            read_ready = FD_ISSET(arr_of_con[i].read_fd, &read_set);
            write_ready = FD_ISSET(arr_of_con[i].write_fd, &write_set);
            
            transfer_data(&arr_of_con[i], read_ready, write_ready, i);
        }   

        FD_ZERO(&read_set);
        FD_ZERO(&write_set);
        tv.tv_sec = SELECT_WAIT;

        for (int i = num_of_closed_con; i < (n -1); i++){

            if ((arr_of_con[i].read_fd_is_closed != 1) && (arr_of_con[i].cur_buf_size == 0)){

                FD_SET(arr_of_con[i].read_fd, &read_set);
                CHECK_MAX_FD(arr_of_con[i].read_fd);
            }

            if (arr_of_con[i].cur_buf_size == 0){
                
                if (arr_of_con[i].read_fd_is_closed == 1){ 
                    
                    close(arr_of_con[i].write_fd);
                    num_of_closed_con++;
                } 
            } else{
                
                FD_SET(arr_of_con[i].write_fd, &write_set);
                CHECK_MAX_FD(arr_of_con[i].write_fd);
            }
        }
    }

    for (int i = 0; i < (n - 1); i++){

        free(arr_of_con[i].buf);
    }

    free(arr_of_con);
}


int main(int argc, char* argv[]){

    CHECK(argc >= 3);

    char* end_of_line = NULL;
    int n = strtol(argv[1], &end_of_line, 10);
    CHECK(end_of_line != NULL);
    CHECK(n >= 1);

    struct fd_for_child* arr_of_pipes = (struct fd_for_child*) calloc(n, sizeof(struct fd_for_child));

    int fd = open(argv[2], O_RDONLY);
    CHECK(fd != -1);

    for (int i = 1; i < (n - 1); i++){

        pipe(arr_of_pipes[i].to_child_pipe);
        pipe(arr_of_pipes[i].from_child_pipe);

        fcntl(arr_of_pipes[i].to_child_pipe[1], F_SETFL, O_NONBLOCK);
        fcntl(arr_of_pipes[i].from_child_pipe[0], F_SETFL, O_NONBLOCK);
    }

    pipe(arr_of_pipes[0].from_child_pipe);
    arr_of_pipes[0].to_child_pipe[0] = fd;
    arr_of_pipes[0].to_child_pipe[1] = -1;

    if (n > 1){

        pipe(arr_of_pipes[n - 1].to_child_pipe);
    } else{

        close(arr_of_pipes[n - 1].from_child_pipe[0]);
        close(arr_of_pipes[n - 1].from_child_pipe[1]);
    }

    arr_of_pipes[n - 1].from_child_pipe[0] = -1;
    arr_of_pipes[n - 1].from_child_pipe[1] = STDOUT_FILENO;

    for (int i = 0; i < n; i++){
        
        pid_t child_id = fork();

        if (child_id == 0){
        
            CLOSE_USELESS_PIPES(i);

            child(i, arr_of_pipes[i].to_child_pipe[0], arr_of_pipes[i].from_child_pipe[1]);
        } else{
            
            close(arr_of_pipes[i].to_child_pipe[0]);
            if (i != (n - 1)){
                close(arr_of_pipes[i].from_child_pipe[1]);
            }
        }
    }  

    parent(fd, arr_of_pipes, n);

    free(arr_of_pipes);
}