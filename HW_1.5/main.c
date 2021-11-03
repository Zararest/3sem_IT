#include <sys/types.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#define MAXLEN 15

enum Msg_types{

    PROC_CREATE = 1,
    SYNC = 2
};


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

void child_stuff(int msgid){

    struct Msg_buf cur_msg;

    msgrcv(msgid, (void*) &cur_msg, MAXLEN, getpid(), 0);

    int number_of_cur_proc = atoi(cur_msg.msg);
    int number_has_printed = 0;
    
    while (number_has_printed == 0){
        
        msgrcv(msgid, (void*) &cur_msg, MAXLEN, SYNC, 0);
        
        if (atoi(cur_msg.msg) == number_of_cur_proc){

            number_has_printed = 1;
            printf("[%i]: %i\n", getpid(), number_of_cur_proc);

            itoa(number_of_cur_proc + 1, &cur_msg.msg);
            msgsnd(msgid, (const void*) &cur_msg, MAXLEN, 0);
        } else{

            msgsnd(msgid, (const void*) &cur_msg, MAXLEN, 0);
        }
    }
    exit(0);
}

void create_proc(char* str){

    void* end_of_line = NULL;
    long n = strtol(str, end_of_line, 10);

    if (n <= 0) exit(-1); 

    struct Msg_buf cur_msg;
    int msgid, number_has_printed = 0;
    key_t key = ftok("./key_file.txt", 10);
    msgid = msgget(key, IPC_CREAT | 0666);
    
    for (int i = 0; i < n; i++){
        //sleep(1);
        pid_t new_child = fork();

        if (new_child == 0){

            child_stuff(msgid);
        } else{

            cur_msg.type = new_child;
            itoa(i, &cur_msg.msg);
            if (msgsnd(msgid, (const void*) &cur_msg, MAXLEN, 0) == -1) printf("snd problem\n");
        }
        
    }
    cur_msg.type = SYNC;
    itoa(0, &cur_msg.msg);
    msgsnd(msgid, (const void*) &cur_msg, MAXLEN, 0);

    while (number_has_printed == 0){
        
        msgrcv(msgid, (void*) &cur_msg, MAXLEN, SYNC, 0);

        if (atoi(cur_msg.msg) == n){

            number_has_printed = 1;
            printf("---------\n");
        } else{

            msgsnd(msgid, (const void*) &cur_msg, MAXLEN, 0);
        }
    }
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