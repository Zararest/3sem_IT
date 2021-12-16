#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdio.h>
#include <sys/select.h>

#define MAXLEN 100

#define CHECK(result)                                   \
    do {                                                \
        if (result < 0) {                               \
            printf ("Error in line: %d\n", __LINE__);   \
        }                                               \
    } while (0) 

int main(){

    char buf[MAXLEN];
    int pipefd[2], i = 0;   //pipe[0] - чтение; pipe[1] - запись
    
    pipe(pipefd);

    CHECK(fcntl(pipefd[0], F_SETFL, O_NONBLOCK));
    CHECK(fcntl(pipefd[1], F_SETFL, O_NONBLOCK));

    fd_set set;
    struct timeval tv = {10, 0};

    FD_ZERO(&set);
    //FD_SET(10, &set);
    close(pipefd[1]);
    FD_SET(pipefd[0], &set);
    printf("select = %i\n", select(11, &set, NULL, NULL, &tv));

    printf("in set %i\n", FD_ISSET(pipefd[1], &set));

    printf("write %li\n", write(pipefd[1], "lol", 3));
    /*
    pid_t child_id = fork();

    if (child_id == 0){
        //тут будем читать
        close(pipefd[1]); 

        for (int i = 0; i < 10; i++){
        
            sleep(1);

            FD_ZERO(&set);
            FD_SET(10, &set);
            tv.tv_sec = 10;
            CHECK(select(11, &set, NULL, NULL, &tv));

            printf("read[%li] number %i\n", read(pipefd[0], buf, sizeof(buf)), i);
        }   

    } else{
        //тут писать
        close(pipefd[0]);
        close(pipefd[1]);
       /* FD_SET(pipefd[1], &set);
        
        while (1){
            i++;

            FD_ZERO(&set);
            FD_SET(pipefd[1], &set);
            tv.tv_sec = 10;
            select(pipefd[1] + 1, NULL, &set, NULL, &tv);

            printf("write[%i] number %i\n", write(pipefd[1], buf, sizeof(buf)), i);
        } */
    //}
    
}