#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <errno.h>

#define MAXLEN 15

enum Msg_types{

    PROC_CREATE = 1,
    SYNC = 2,
    ORDER_NUMBER = 3
};

struct Msg_buf{

    long type;

    char msg[MAXLEN];
};

void child_stuff(int msgid, int number_of_childs){

    struct Msg_buf cur_msg;
    cur_msg.type = PROC_CREATE;
    itoa(pid(), cur_msg.msg, 10);

    msgsnd(msgid, (const void*) &cur_msg, MAXLEN, 0);

    msgrcv(msgid, (void*) &cur_msg, MAXLEN, SYNC, 0);

    int msg_int = atoi(cur_msg.msg);
    if (msg_int != number_of_childs){

        itoa(++msg_int, cur_msg.msg, 10);
        msgsnd(msgid, (const void*) &cur_msg, MAXLEN, 0);
    }

    for (int i = 0; i < number_of_childs; i++){

        //читаем  все сообщения PROC_CREATE
        //посылаем номер процессора с типом равным типу сообщения
    }

    //получаем сообщение с типом id
    //синхронизируем через тип sync
}

void create_proc(char* str){

    void* end_of_line = NULL;
    long n = strtol(str, end_of_line, 10);

    if (n <= 0) exit(-1); 

    int msgid, parent_proc = 1;
    key_t key = ftok("./key_file.txt", 10);
    msgid = msgget(key, IPC_CREAT);

    for (int i = 0; i < n; i++){

        if (parent_proc == 1){

            pid_t new_child = fork();

            if (new_child == 0){

                child_stuff(msgid, n);
                parent_proc = 0;
            }
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

    printf("\n");
}