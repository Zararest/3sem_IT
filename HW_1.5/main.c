#include <sys/types.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#define MAXLEN 15
#define START_PRINTING 1

struct Msg_buf{

    long type;

    char msg[MAXLEN];
};

int len(int num){

    if (num == 0) return 1;

    int counter = 0;

    while (num != 0){

        num = num / 10;
        counter++;
    }

    return counter;
}

int itoa(int num, char* str){

    if (str == NULL) return -1;

    char number = 0;
    int num_len = len(num);

    for (int i = 1; i <= num_len; i++){

        number = num % 10;
        num = num / 10;
        str[num_len - i] = '0' + number;
    }
    str[num_len] = '\0';
    return 0;
}

void child_stuff(int msg_data, int msg_sync){

    struct Msg_buf cur_msg;
    char str_out[MAXLEN];

    msgrcv(msg_data, (void*) &cur_msg, MAXLEN, getpid(), 0);
    int number_of_cur_proc = atoi(cur_msg.msg);
    
    msgrcv(msg_sync, (void*) &cur_msg, MAXLEN, number_of_cur_proc, 0);
    printf("%i ", number_of_cur_proc);
    //fflush (stdout);
    sprintf(str_out, "%i ", number_of_cur_proc);   //критическая секция 
    write(STDOUT_FILENO, str_out, strlen(str_out));//гонка за место в буфере ядра на вывод

    cur_msg.type = number_of_cur_proc + 1;
    msgsnd(msg_sync, (const void*) &cur_msg, MAXLEN, 0);
    
    exit(0);
}

void create_proc(char* str){

    void* end_of_line = NULL;
    long n = strtol(str, end_of_line, 10);

    if (n <= 0) exit(-1); 

    struct Msg_buf cur_msg;
    int msg_data, msg_sync;
    //key_t key_data = ftok("./key_file.txt", 10), key_sync = ftok("./key_file.txt", 11);

    msg_data = msgget(IPC_PRIVATE, IPC_CREAT | 0666);
    msg_sync = msgget(IPC_PRIVATE, IPC_CREAT | 0666);
    
    if ((msg_data == -1) || (msg_sync == -1)){

        printf("Can't create msg\n");
        exit(0);
    }

    for (int i = 1; i <= n; i++){
        
        pid_t new_child = fork();

        if (new_child == 0){

            child_stuff(msg_data, msg_sync);
        } else{

            cur_msg.type = new_child;
            itoa(i, (char*) &cur_msg.msg); //заменить на sprintf
            if (msgsnd(msg_data, (const void*) &cur_msg, MAXLEN, 0) == -1) printf("snd problem\n");
        }
        
    }
    cur_msg.type = START_PRINTING;
    msgsnd(msg_sync, (const void*) &cur_msg, MAXLEN, 0);
        
    msgrcv(msg_sync, (void*) &cur_msg, MAXLEN, n + 1, 0);
    printf("\n---------\n");
    msgctl(msg_data, IPC_RMID, NULL);
    msgctl(msg_sync, IPC_RMID, NULL);
}


int main(int argc, char* argv[]){

    if (argc >= 2){

        char* end_of_line = NULL;
        strtol(argv[1], &end_of_line, 10);

        if ((errno == EINVAL) || (errno == ERANGE) || (*end_of_line != '\0')){

            printf("Input error\n");  

        } else{
            
            create_proc(argv[1]);
        }
    } else{

        printf("Not enogh args\n");
    }
}